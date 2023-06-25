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
#include <unordered_map>
#include <stack>
#include "Utils.h"
#include "ccerr.h"
#include "declcpp.h"
#include "config.h"
#include "mangle.h"
#include "initbackend.h"
#include "occparse.h"
#include "template.h"
#include "lambda.h"
#include "help.h"
#include "stmt.h"
#include "expr.h"
#include "lex.h"
#include "cpplookup.h"
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
#include "constopt.h"
#include "constexpr.h"
#include "optmain.h"
#include "iexpr.h"
#include "iblock.h"
#include "istmt.h"
#include "iinline.h"
#include "symtab.h"
#include "ListFactory.h"
#include "inline.h"

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

static bool MustSpecialize(const char* name)
{
    if (noNeedToSpecialize || (templateNestingCount && !instantiatingTemplate))
        return false;
    for (auto&& sst : structSyms)
    {
        if (sst.str && !strcmp(sst.str->name, name))
            return false;
    }
    return true;
}
void SpecializationError(char* str)
{
    if (MustSpecialize(str))
        errorstr(ERR_NEED_TEMPLATE_ARGUMENTS, str);
}

void SpecializationError(SYMBOL* sym)
{
    if (MustSpecialize(sym->name))
        errorsym(ERR_NEED_TEMPLATE_ARGUMENTS, sym);
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
                            if (func->sb->deferredCompile && (!func->sb->templateLevel || func->sb->instantiated))
                            {
                                FUNCTIONCALL fcall = {};
                                TYPE* tp = nullptr;
                                EXPRESSION* exp = intNode(en_c_i, 0);
                                SYMBOL* sp = func->sb->overloadName;
                                fcall.arguments = initListListFactory.CreateList();
                                for (auto sym : *basetype(func->tp)->syms)
                                {
                                    if (sym->sb->thisPtr)
                                    {
                                        fcall.thistp = sym->tp;
                                        fcall.thisptr = exp;
                                    }
                                    else if (sym->tp->type != bt_void)
                                    {
                                        auto arg = Allocate<INITLIST>();
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
                                strcpy(buf, sym->sb->decoratedName);
                                Optimizer::my_sprintf(buf + strlen(buf), "_@%c%d", count % 26 + 'A', count / 26);

                                thunks[count].entry = entry;
                                if (func->sb->attribs.inheritable.linkage2 == lk_import)
                                {
                                    EXPRESSION* exp = varNode(en_pc, func);
                                    thunkForImportTable(&exp);
                                    thunks[count].func = exp->v.sp;
                                }
                                else
                                {
                                    thunks[count].func = func;
                                }
                                thunks[count].name = localsp = makeID(sc_static, &stdfunc, nullptr, litlate(buf));
                                localsp->sb->decoratedName = localsp->name;
                                localsp->sb->attribs.inheritable.linkage4 = lk_virtual;
                                Optimizer::genref(Optimizer::SymbolManager::Get(localsp), 0);
                                count++;
                            }
                            else
                            {
                                if (func->sb->attribs.inheritable.linkage2 == lk_import)
                                {
                                    EXPRESSION* exp = varNode(en_pc, func);
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
        SYMBOL* xtSym = RTTIDumpType(basetype(sym->tp));
        int count = 0;

        Optimizer::dseg();
        Optimizer::gen_virtual(Optimizer::SymbolManager::Get(sym->sb->vtabsp), true);
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
                Optimizer::gen_virtual(Optimizer::SymbolManager::Get(thunks[i].name), false);
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
    if (base == derived || (base && derived && sameTemplate(derived->tp, base->tp)))
    {

        if (!templateNestingCount || instantiatingTemplate)
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
                if (exactMatchOnTemplateArgs(derived->templateParams,
                                             base->templateParams))
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
                    if (sym->tp->type == bt_typedef)
                    {
                        sym = basetype(sym->tp)->sp;
                    }
                    if (sym == base)
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
    if (derived && derived->sb && derived->sb->mainsym)
        derived = derived->sb->mainsym;
    internalClassRefCount(base, derived, &vcount, &ccount, false);
    if (vcount)
        ccount++;
    return ccount;
}
static bool vfMatch(SYMBOL* sym, SYMBOL* oldFunc, SYMBOL* newFunc)
{
    bool rv = false;
    rv = !strcmp(oldFunc->name, newFunc->name) && matchOverload(oldFunc->tp, newFunc->tp, false);
    if (rv && !oldFunc->sb->isDestructor)
    {
        TYPE *tp1 = basetype(oldFunc->tp)->btp, *tp2 = basetype(newFunc->tp)->btp;
        if (!comparetypes(tp1, tp2, true) && !sameTemplate(tp1, tp2) &&
            (!isstructured(tp1) || !isstructured(tp2) ||
             (basetype(tp1)->sp->sb->templateLevel &&
              basetype(tp2)->sp->sb->parentTemplate != basetype(tp1)->sp->sb->parentTemplate)))
        {
            if (!templateNestingCount)
            {
                if (basetype(tp1)->type == basetype(tp2)->type)
                {
                    if (ispointer(tp1) || isref(tp1))
                    {
                        if (isconst(tp1) == isconst(tp2) && isvolatile(tp1) == isvolatile(tp2))
                        {
                            tp1 = basetype(tp1)->btp;
                            tp2 = basetype(tp2)->btp;
                            if (isstructured(tp1) && isstructured(tp2))
                            {
                                if ((!isconst(tp1) || isconst(tp2)) && (!isvolatile(tp1) || isvolatile(tp2)))
                                {
                                    tp1 = basetype(tp1);
                                    tp2 = basetype(tp2);
                                    if (tp1->sp != tp2->sp && tp2->sp != sym && classRefCount(tp1->sp, tp2->sp) != 1)
                                    {
                                        errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, oldFunc->sb->parentClass, newFunc->sb->parentClass);
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
                        offset += getSize(bt_pointer);
                    }
                offset += 2 * getSize(bt_pointer);
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
            (*pos)-> push_back(vt);

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
                    for (;itm != itme && itv != itve; ++itm, ++itv)
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
                if (comparetypes((TYPE*)l2->data, (TYPE*)l1->data, true) && intcmp((TYPE*)l2->data, (TYPE*)l1->data))
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
    for (auto sym : *basetype(sp->tp)->syms)
    {
        if (sym->sb->storage_class == sc_overloads)
        {
            for (auto sym1 : *basetype(sym->tp)->syms)
            {
                if (sym1->sb->storage_class == sc_virtual)
                {
                    const char* f1 = (char*)strrchr(sym1->sb->decoratedName, '@');
                    if (f1 && sp->sb->baseClasses)
                    {
                        for (auto bc : *sp->sb->baseClasses)
                        {

                            SYMBOL* sym2 = search(basetype(bc->cls->tp)->syms, sym->name);
                            if (sym2)
                            {
                                for (auto sym3 : *basetype(sym2->tp)->syms)
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
            if (cur->sb->storage_class == sc_overloads)
            {
                for (auto cur1 : *cur->tp->syms)
                {
                    VTABENTRY *hold, *hold2;
                    bool found = false;
                    bool isfirst = false;
                    bool isvirt = cur1->sb->storage_class == sc_virtual;
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
                        cur1->sb->storage_class = sc_virtual;
                        if (!isfirst)
                        {
                            if (sym->sb->vtabEntries->front()->virtuals == nullptr)
                                sym->sb->vtabEntries->front()->virtuals = symListFactory.CreateList();
                            sym->sb->vtabEntries->front()->virtuals->push_back(cur1);
                        }
                    }
                    if (cur1->sb->isoverride && !found && !isfirst)
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
            ofs += getSize(bt_pointer);
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
                align = getBaseAlign(bt_pointer);
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
                sym->tp->size += getSize(bt_pointer);
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
    LEXLIST* lex;
    STRUCTSYM l, n, x, q;
    int count = 0;
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

        cur->sb->attribs.inheritable.linkage4 = lk_virtual;
        if (cur->templateParams && cur->sb->templateLevel)
        {
            n.tmpl = cur->templateParams;
            addTemplateDeclaration(&n);
            count++;
        }
        if (cur->sb->parentClass)
        {
            l.str = cur->sb->parentClass;
            addStructureDeclaration(&l);
            count++;
            if (cur->sb->parentClass->templateParams)
            {
                q.tmpl = cur->sb->parentClass->templateParams;
                addTemplateDeclaration(&q);
                count++;
            }
        }
        dontRegisterTemplate++;
        lex = SetAlternateLex(cur->sb->deferredCompile);
        if (MATCHKW(lex, kw_try) || MATCHKW(lex, colon))
        {
            bool viaTry = MATCHKW(lex, kw_try);
            if (viaTry)
            {
                cur->sb->hasTry = true;
                lex = getsym();
            }
            if (MATCHKW(lex, colon))
            {
                lex = getsym();
                cur->sb->memberInitializers = GetMemberInitializers(&lex, nullptr, cur);
            }
        }
        oldLambdas = lambdas;
        lambdas.clear();
        cur->sb->deferredCompile = nullptr;
        lex = body(lex, cur);
        SetAlternateLex(nullptr);
        dontRegisterTemplate--;
        lambdas = oldLambdas;
        while (count--)
        {
            dropStructureDeclaration();
        }
        PopTemplateNamespace(tns);
        lines = linesOld;
    }
}
static void RecalcArraySize(TYPE* tp)
{
    if (isarray(basetype(tp)->btp))
        RecalcArraySize(basetype(tp)->btp);
    tp->size = basetype(basetype(tp)->btp)->size * basetype(tp)->esize->v.i;
}
void deferredInitializeDefaultArg(SYMBOL* arg, SYMBOL* func)
{
    if (!templateNestingCount || instantiatingTemplate)
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
                templateNestingCount++;
            }
            LEXLIST* lex;
            STRUCTSYM l, n, q;
            TYPE* tp2;
            int count = 0;
            int tns = PushTemplateNamespace(str ? str : func);
            if (str)
            {
                l.str = str;
                addStructureDeclaration(&l);
                count++;
                if (str->templateParams)
                {
                    n.tmpl = str->templateParams;
                    addTemplateDeclaration(&n);
                    count++;
                }
            }
            if (func->templateParams)
            {
                q.tmpl = func->templateParams;
                addTemplateDeclaration(&q);
                count++;
            }
            //        func->tp = PerformDeferredInitialization(func->tp, nullptr);
            lex = SetAlternateLex(arg->sb->deferredCompile);

            arg->tp = PerformDeferredInitialization(arg->tp, nullptr);
            tp2 = arg->tp;
            if (isref(tp2))
                tp2 = basetype(tp2)->btp;
            if (isstructured(tp2))
            {
                SYMBOL* sym2;
                anonymousNotAlloc++;
                sym2 = anonymousVar(sc_auto, tp2)->v.sp;
                anonymousNotAlloc--;
                sym2->sb->stackblock = !isref(arg->tp);
                lex = initialize(lex, theCurrentFunc, sym2, sc_auto, false, false, 0); /* also reserves space */
                arg->sb->init = sym2->sb->init;
                if (arg->sb->init->front()->exp->type == en_thisref)
                {
                    EXPRESSION** expr = &arg->sb->init->front()->exp->left->v.func->thisptr;
                    if ((*expr)->type == en_add && isconstzero(&stdint, (*expr)->right))
                        arg->sb->init->front()->exp->v.t.thisptr = (*expr) = (*expr)->left;
                }
            }
            else
            {
                lex = initialize(lex, theCurrentFunc, arg, sc_member, false, false, 0);
            }
            SetAlternateLex(nullptr);
            arg->sb->deferredCompile = nullptr;
            while (count--)
            {
                dropStructureDeclaration();
            }
            PopTemplateNamespace(tns);
            if (initted)
            {
                templateNestingCount--;
                instantiatingTemplate = oldInstantiatingTemplate;
            }
        }
    }
}
void deferredInitializeStructFunctions(SYMBOL* cur)
{
    LEXLIST* lex;
    STRUCTSYM l, n;
    int count = 0;
    int tns = PushTemplateNamespace(cur);
    l.str = cur;
    addStructureDeclaration(&l);
    count++;

    if (cur->templateParams)
    {
        n.tmpl = cur->templateParams;
        addTemplateDeclaration(&n);
        count++;
    }
    //    dontRegisterTemplate++;
    for (auto sp : *cur->tp->syms)
    {
        if (sp->sb->storage_class == sc_overloads)
        {
            if (templateNestingCount != 1 || instantiatingTemplate)
            {
                for (auto sp1 : *sp->tp->syms)
                {
                    if (!sp1->sb->templateLevel)
                    {
                        for (auto sp2 : *basetype(sp1->tp)->syms)
                        {
                            if (!sp2->sb->thisPtr)
                            {
                                sp2->tp = PerformDeferredInitialization(sp2->tp, nullptr);
                                if (sp2->sb->deferredCompile && !sp2->sb->init)
                                {
                                    TYPE* tp2;
                                    lex = SetAlternateLex(sp2->sb->deferredCompile);
                                    tp2 = sp2->tp;
                                    if (isref(tp2))
                                        tp2 = basetype(tp2)->btp;
                                    if (!isstructured(tp2))
                                    {
                                        lex = initialize(lex, theCurrentFunc, sp2, sc_member, false, false, 0);
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
    while (count--)
    {
        dropStructureDeclaration();
    }
    PopTemplateNamespace(tns);
}
void deferredInitializeStructMembers(SYMBOL* cur)
{
    Optimizer::LIST* staticAssert;
    LEXLIST* lex;
    STRUCTSYM l, n;
    int count = 0;
    int tns = PushTemplateNamespace(cur);
    l.str = cur;
    addStructureDeclaration(&l);
    count++;

    if (cur->templateParams)
    {
        n.tmpl = cur->templateParams;
        addTemplateDeclaration(&n);
        count++;
    }
    dontRegisterTemplate++;
    for (auto sp : *cur->tp->syms)
    {
        if (isarray(sp->tp) && sp->tp->esize)
        {
            RecalcArraySize(sp->tp);
        }
        if (sp->sb->storage_class == sc_overloads)
        {
        }
        else if (sp->sb->deferredCompile && !sp->sb->init)
        {
            lex = SetAlternateLex(sp->sb->deferredCompile);
            sp->sb->deferredCompile = nullptr;
            lex = initialize(lex, theCurrentFunc, sp, sc_member, false, false, 0);
            SetAlternateLex(nullptr);
        }
    }
    dontRegisterTemplate--;
    while (count--)
    {
        dropStructureDeclaration();
    }
    PopTemplateNamespace(tns);
}
bool declaringTemplate(SYMBOL* sym)
{
    for (auto&& l : structSyms)
    {
        if (l.str && l.str->sb->templateLevel)
        {
            if (sym->sb->decoratedName && !strcmp(sym->sb->decoratedName, l.str->sb->decoratedName))
                return true;
        }
    }
    return false;
}
TYPE* PerformDeferredInitialization(TYPE* tp, SYMBOL* funcsp)
{
    (void)funcsp;
    if (!tp)
        return &stdany;
    TYPE** tpx = &tp;
    if (isref(*tpx))
        tpx = &basetype(*tpx)->btp;
    TYPE** tpx1 = tpx;
    while ((*tpx)->btp && !isfunction(*tpx))
        tpx = &(*tpx)->btp;
    if (Optimizer::cparams.prm_cplusplus && !inTemplateType && isstructured(*tpx))
    {
        SYMBOL* sym = basetype(*tpx)->sp;
        if (sym->templateParams)
            for (auto&& tpl : *sym->templateParams)
                if (tpl.second->usedAsUnpacked)
                    return tp;
        if (declaringTemplate(sym))
        {
            if (!sym->sb->instantiated)
            {
                *tpx = sym->tp;
            }
        }
        else if (sym->sb->templateLevel && (!sym->sb->instantiated || sym->sb->attribs.inheritable.linkage4 != lk_virtual) &&
                 sym->templateParams && allTemplateArgsSpecified(sym, sym->templateParams, false, true))
        {
            sym = TemplateClassInstantiateInternal(sym, nullptr, false);
            if (sym)
                *tpx = sym->tp;
        }
        else if (!sym->sb->templateLevel && sym->sb->parentClass && sym->sb->parentClass->sb->templateLevel &&
                 (!sym->sb->instantiated || sym->sb->attribs.inheritable.linkage4 != lk_virtual) &&
                 sym->sb->parentClass->templateParams &&
                 allTemplateArgsSpecified(sym->sb->parentClass, sym->sb->parentClass->templateParams, false, true))
        {
            std::list<TEMPLATEPARAMPAIR>* tpl = sym->sb->parentClass->templateParams;
            sym->templateParams = tpl;
            sym = TemplateClassInstantiateInternal(sym, nullptr, false);
            sym->templateParams = nullptr;
            if (sym)
                *tpx = sym->tp;
        }
        else if (!sym->sb->instantiated || ((*tpx)->size < sym->tp->size && sym->tp->size != 0))
        {
            *tpx = sym->tp;
        }
        UpdateRootTypes(tp);
    }
    return tp;
}
void warnCPPWarnings(SYMBOL* sym, bool localClassWarnings)
{
    for (auto cur : *sym->tp->syms)
    {
        if (cur->sb->storage_class == sc_static && (cur->tp->hasbits || localClassWarnings))
            errorstr(ERR_INVALID_STORAGE_CLASS, "static");
        if (sym != cur && !strcmp(sym->name, cur->name))
        {
            if (sym->sb->hasUserCons || cur->sb->storage_class == sc_static || cur->sb->storage_class == sc_overloads ||
                cur->sb->storage_class == sc_const || cur->sb->storage_class == sc_type)
            {
                errorsym(ERR_MEMBER_SAME_NAME_AS_CLASS, sym);
                break;
            }
        }
        if (cur->sb->storage_class == sc_overloads)
        {
            for (auto cur1 : *cur->tp->syms)
            {
                if (localClassWarnings)
                {
                    if (isfunction(cur1->tp))
                        if (!basetype(cur1->tp)->sp->sb->inlineFunc.stmt)
                            errorsym(ERR_LOCAL_CLASS_FUNCTION_NEEDS_BODY, cur1);
                }
                if (cur1->sb->isfinal || cur1->sb->isoverride || cur1->sb->ispure)
                    if (cur1->sb->storage_class != sc_virtual)
                        error(ERR_SPECIFIER_VIRTUAL_FUNC);
            }
        }
    }
}
bool usesVTab(SYMBOL* sym)
{
    BASECLASS* base;
    for (auto cur : *sym->tp->syms)
    {
        if (cur->sb->storage_class == sc_overloads)
        {
            for (auto sp : *cur->tp->syms)
            {
                if (sp->sb->storage_class == sc_virtual)
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
BASECLASS* innerBaseClass(SYMBOL* declsym, SYMBOL* bcsym, bool isvirtual, enum e_ac currentAccess)
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
    if (basetype(bcsym->tp)->type == bt_union)
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
        injected->name = injected->sb->decoratedName;  // for nested templates
        injected->sb->mainsym = bcsym;
        injected->sb->parentClass = declsym;
        injected->sb->access = currentAccess;
        declsym->tp->tags->Add(injected);
    }
    return bc;
}
LEXLIST* baseClasses(LEXLIST* lex, SYMBOL* funcsp, SYMBOL* declsym, enum e_ac defaultAccess)
{
    auto baseClasses = baseClassListFactory.CreateList();
    enum e_ac currentAccess;
    bool isvirtual = false;
    bool done = false;
    SYMBOL* bcsym;
    STRUCTSYM l;
    currentAccess = defaultAccess;
    lex = getsym();  // past ':'
    if (basetype(declsym->tp)->type == bt_union)
        error(ERR_UNION_CANNOT_HAVE_BASE_CLASSES);
    l.str = (SYMBOL*)declsym;
    addStructureDeclaration(&l);
    do
    {
        ParseAttributeSpecifiers(&lex, funcsp, true);
        if (MATCHKW(lex, kw_decltype))
        {
            TYPE* tp = nullptr;
            lex = get_type_id(lex, &tp, funcsp, sc_type, true, true, false);
            if (!tp)
            {
                error(ERR_TYPE_NAME_EXPECTED);
            }
            else if (!isstructured(tp))
            {
                error(ERR_STRUCTURED_TYPE_EXPECTED);
            }
            else
            {
                auto bc = innerBaseClass(declsym, tp->sp, isvirtual, currentAccess);
                if (bc)
                    baseClasses->push_back(bc);
            }
            done = !MATCHKW(lex, comma);
            if (!done)
                lex = getsym();
        }
        else if (MATCHKW(lex, classsel) || ISID(lex))
        {
            char name[512];
            name[0] = 0;
            if (ISID(lex))
                strcpy(name, lex->data->value.s.a);
            bcsym = nullptr;
            lex = nestedSearch(lex, &bcsym, nullptr, nullptr, nullptr, nullptr, false, sc_global, false, false);
            if (bcsym && bcsym->sb && bcsym->sb->storage_class == sc_typedef)
            {
                if (!bcsym->sb->templateLevel)
                {
                    // in case typedef is being used as a base class specifier
                    TYPE* tp = basetype(bcsym->tp);
                    if (isstructured(tp))
                    {
                        bcsym = tp->sp;
                    }
                    else if (tp->type != bt_templateselector)
                    {
                        bcsym = nullptr;
                    }
                }
            }
            lex = getsym();
        restart:
            if (bcsym && bcsym->tp->type == bt_templateselector)
            {
                if (!templateNestingCount)
                    error(ERR_STRUCTURED_TYPE_EXPECTED_IN_TEMPLATE_PARAMETER);
                if (MATCHKW(lex, lt))
                {
                    inTemplateSpecialization++;
                    std::list<TEMPLATEPARAMPAIR>* nullLst;
                    lex = GetTemplateArguments(lex, funcsp, bcsym, &nullLst);
                    inTemplateSpecialization--;
                }
                bcsym = nullptr;
            }
            else if (bcsym && (bcsym->sb && bcsym->sb->templateLevel ||
                               bcsym->tp->type == bt_templateparam && bcsym->tp->templateParam->second->type == kw_template))
            {
                if (bcsym->tp->type == bt_templateparam)
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
                        done = !MATCHKW(lex, comma);
                        if (!done)
                            lex = getsym();
                        goto endloop;
                    }
                }
                if (bcsym->sb->storage_class == sc_typedef)
                {
                    if (MATCHKW(lex, lt))
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
                            if (isstructured(bcsym->tp))
                                bcsym->tp = PerformDeferredInitialization(bcsym->tp, funcsp);
                            else
                                bcsym->tp = SynthesizeType(bcsym->tp, nullptr, false);
                            if (templateNestingCount && bcsym->tp->type == bt_any)
                            {
                                currentAccess = defaultAccess;
                                isvirtual = false;
                                done = !MATCHKW(lex, comma);
                                if (!done)
                                    lex = getsym();
                                continue;
                            }
                        }
                    }
                    else
                    {
                        SpecializationError(bcsym);
                    }
                }
                else
                {
                    std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                    if (MATCHKW(lex, lt))
                    {
                        int i;
                        inTemplateSpecialization++;
                        lex = GetTemplateArguments(lex, funcsp, bcsym, &lst);
                        inTemplateSpecialization--;
                        if (MATCHKW(lex, ellipse))
                        {
                            if (templateNestingCount)
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
                                    for ( ;itsrc != itsrce && itdest != itdeste; ++itsrc, ++itdest)
                                    {
                                        auto second = Allocate<TEMPLATEPARAM>();
                                        workingList->push_back(TEMPLATEPARAMPAIR{ nullptr, second });
                                        if (itsrc->second->packed && itsrc->second->byPack.pack)
                                        {
                                            auto itp = itsrc->second->byPack.pack->begin();
                                            auto itpe = itsrc->second->byPack.pack->end();
                                            packed = true;
                                            for (i = 0; i < n && itp != itpe; i++, ++itp);
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
                                for  (;itsrc != itsrce && itdest != itdeste; ++itdest)
                                {
                                    if (itsrc->second->packed && itsrc->second->byPack.pack)
                                    {
                                        auto itp = itsrc->second->byPack.pack->begin();
                                        auto itpe = itsrc->second->byPack.pack->end();
                                        for (i = 0; i < n && itp != itpe; i++, ++itp);
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
                            done = !MATCHKW(lex, comma);
                            if (!done)
                                lex = getsym();
                            continue;
                        }
                        else
                        {
                            bcsym = GetClassTemplate(bcsym, lst, true);
                            if (bcsym && bcsym->sb->attribs.inheritable.linkage4 != lk_virtual &&
                                allTemplateArgsSpecified(bcsym, bcsym->templateParams))
                            {
                                bcsym->tp = TemplateClassInstantiateInternal(bcsym, bcsym->templateParams, false)->tp;
                                bcsym->tp->sp = bcsym;
                            }
                        }
                    }
                }
            }
            else if (MATCHKW(lex, lt))
            {
                errorstr(ERR_NOT_A_TEMPLATE, bcsym ? bcsym->name : name[0] ? name : "unknown");
            }
            if (bcsym && bcsym->tp->templateParam && bcsym->tp->templateParam->second->packed)
            {
                if (bcsym->tp->templateParam->second->type != kw_typename)
                    error(ERR_NEED_PACKED_TEMPLATE_OF_TYPE_CLASS);
                else if (bcsym->tp->templateParam->second->byPack.pack)
                {
                    for (auto tpp : *bcsym->tp->templateParam->second->byPack.pack)
                    {
                        if (!isstructured(tpp.second->byClass.val))
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
                if (!MATCHKW(lex, ellipse))
                    error(ERR_PACK_SPECIFIER_REQUIRED_HERE);
                else
                    lex = getsym();
                currentAccess = defaultAccess;
                isvirtual = false;
                done = !MATCHKW(lex, comma);
                if (!done)
                    lex = getsym();
            }
            else if (bcsym && bcsym->tp->templateParam && !bcsym->tp->templateParam->second->packed)
            {
                if (bcsym->tp->templateParam->second->type != kw_typename)
                    error(ERR_CLASS_TEMPLATE_PARAMETER_EXPECTED);
                else
                {
                    TYPE* tp = bcsym->tp->templateParam->second->byClass.val;
                    if (tp)
                    {
                        tp = basetype(tp);
                        if (tp->type == bt_templateselector)
                        {
                            SYMBOL* sym = (*tp->sp->sb->templateSelector)[1].sp;
                            for (int i = 2; i < (*tp->sp->sb->templateSelector).size() && sym; ++i)
                            {
                                PerformDeferredInitialization(sym->tp, funcsp);
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
                    if (!tp || !isstructured(tp))
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
                done = !MATCHKW(lex, comma);
                if (!done)
                    lex = getsym();
            }
            else if (bcsym && (istype(bcsym) && isstructured(bcsym->tp)))
            {
                auto bc = innerBaseClass(declsym, bcsym, isvirtual, currentAccess);
                if (bc)
                    baseClasses->push_back(bc);
                currentAccess = defaultAccess;
                isvirtual = false;
                done = !MATCHKW(lex, comma);
                if (!done)
                    lex = getsym();
            }
            else
            {
                if (!templateNestingCount)
                    error(ERR_CLASS_TYPE_EXPECTED);
                done = true;
            }
        }
        else
            switch (KW(lex))
            {
                case kw_virtual:
                    isvirtual = true;
                    lex = getsym();
                    break;
                case kw_private:
                    currentAccess = ac_private;
                    lex = getsym();
                    break;
                case kw_protected:
                    currentAccess = ac_protected;
                    lex = getsym();
                    break;
                case kw_public:
                    currentAccess = ac_public;
                    lex = getsym();
                    break;
                default:
                    error(ERR_IDENTIFIER_EXPECTED);
                    errskim(&lex, skim_end);
                    dropStructureDeclaration();
                    return lex;
            }
    endloop:
        if (!done)
            ParseAttributeSpecifiers(&lex, funcsp, true);
    } while (!done);
    dropStructureDeclaration();

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
                    if (isAccessible(lst2->cls, lst2->cls, lst->cls, nullptr, ac_protected, false))
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
static bool hasPackedTemplate(TYPE* tp)
{

    switch (tp->type)
    {
        case bt_typedef:
            break;
        case bt_aggregate:
            tp = tp->syms->front()->tp;
            /* fall through */
        case bt_func:
        case bt_ifunc:
            if (hasPackedTemplate(tp->btp))
                return true;
            if (tp->syms)
            {
                for (auto sym : *tp->syms)
                {
                    if (hasPackedTemplate(sym->tp))
                        return true;
                }
            }
            break;
        case bt_float_complex:
            break;
        case bt_double_complex:
            break;
        case bt_long_double_complex:
            break;
        case bt_float_imaginary:
            break;
        case bt_double_imaginary:
            break;
        case bt_long_double_imaginary:
            break;
        case bt_float:
            break;
        case bt_double:
            break;
        case bt_long_double:
            break;
        case bt_unsigned:
        case bt_int:
            break;
        case bt_char16_t:
            break;
        case bt_char32_t:
            break;
        case bt_unsigned_long_long:
        case bt_long_long:
            break;
        case bt_unsigned_long:
        case bt_long:
            break;
        case bt_wchar_t:
            break;
        case bt_unsigned_short:
        case bt_short:
            break;
        case bt_unsigned_char:
        case bt_signed_char:
        case bt_char:
            break;
        case bt_inative:
        case bt_unative:
            break;
        case bt_bool:
            break;
        case bt_bit:
            break;
        case bt_void:
            break;
        case bt_pointer:
        case bt_memberptr:
        case bt_const:
        case bt_volatile:
        case bt_lref:
        case bt_rref:
        case bt_lrqual:
        case bt_rrqual:
        case bt_derivedfromtemplate:
            return hasPackedTemplate(tp->btp);
        case bt_seg:
            break;
        case bt_ellipse:
            break;
        case bt_any:
            break;
        case bt___string:
            break;
        case bt___object:
            break;
        case bt_class:
            break;
        case bt_struct:
            break;
        case bt_union:
            break;
        case bt_enum:
            break;
        case bt_templateparam:
            return tp->templateParam->second->packed;
        default:
            diag("hasPackedTemplateParam: unknown type");
            break;
    }
    return 0;
}
void checkPackedType(SYMBOL* sym)
{
    if (sym->sb->storage_class != sc_parameter)
    {
        error(ERR_PACK_SPECIFIER_MUST_BE_USED_IN_PARAMETER);
    }
}
std::stack<EXPRESSION*> iterateToPostOrder(EXPRESSION* exp)
{
    std::stack<EXPRESSION*> st1;
    st1.push(exp);
    std::stack<EXPRESSION*> st2;
    while (!st1.empty())
    {
        EXPRESSION* curr = st1.top();
        st1.pop();
        st2.push(curr);
        if (curr->left)
        {
            st1.push(curr->left);
        }
        if (curr->right)
        {
            st1.push(curr->right);
        }
    }
    return st2;
}
bool hasPackedExpression(EXPRESSION* exp, bool useAuto)
{
    if (!exp)
        return false;
    std::stack<EXPRESSION*> st = iterateToPostOrder(exp);
    while (!st.empty())
    {
        EXPRESSION* exp1 = st.top();
        st.pop();
        if (useAuto && exp1->type == en_auto)
        {
            if (exp1->v.sp->packed)
            {
                return true;
            }
            else
            {
                continue;
            }
        }
        if (exp1->type == en_func)
        {
            if (useAuto)
            {
                if (exp1->v.func->templateParams)
                    for (auto&& tpl : *exp1->v.func->templateParams)
                        if (tpl.second->packed)
                            return true;
            }
            if (exp1->v.func->arguments)
                for (auto il : *exp1->v.func->arguments)
                    if (hasPackedExpression(il->exp, useAuto))
                        return true;
            if (exp1->v.func->thisptr && hasPackedExpression(exp1->v.func->thisptr, useAuto))
                return true;
        }
        if (exp1->type == en_templateparam)
        {
            if (exp1->v.sp->tp->templateParam->second->packed)
                return true;
        }
    }
    return false;
}
void checkPackedExpression(EXPRESSION* exp)
{
    if (!hasPackedExpression(exp, true) && !templateNestingCount)
        error(ERR_PACK_SPECIFIER_REQUIRES_PACKED_FUNCTION_PARAMETER);
}
void checkUnpackedExpression(EXPRESSION* exp)
{
    if (hasPackedExpression(exp, false))
        error(ERR_PACK_SPECIFIER_REQUIRED_HERE);
}
void GatherPackedVars(int* count, SYMBOL** arg, EXPRESSION* packedExp);
void GatherPackedTypes(int* count, SYMBOL** arg, TYPE* tp);
void GatherTemplateParams(int* count, SYMBOL** arg, std::list<TEMPLATEPARAMPAIR>* tplx)
{
    if (tplx)
    {
        for (auto&& tpl : *tplx)
        {
            if (tpl.second->packed && tpl.first && tpl.second->type == kw_typename)
            {
                arg[(*count)++] = /*sym*/ tpl.first;
                NormalizePacked(tpl.first->tp);
            }
            else if (tpl.second->type == kw_int)
            {
                if (tpl.second->byNonType.dflt)
                {
                    if (tpl.second->packed)
                    {
                        if (tpl.first)
                            arg[(*count)++] = tpl.first;
                        if (tpl.second->byPack.pack)
                            for (auto&& tpl1 : *tpl.second->byPack.pack)
                                GatherPackedVars(count, arg, tpl1.second->byNonType.dflt);
                    }
                    else
                    {
                        GatherPackedVars(count, arg, tpl.second->byNonType.dflt);
                    }
                }
            }
            else if (tpl.second->type == kw_typename)
            {
                if (tpl.second->byClass.dflt)
                {
                    if (tpl.second->packed)
                    {
                        if (tpl.second->byPack.pack)
                            for (auto&& tpl1 : *tpl.second->byPack.pack)
                                GatherPackedTypes(count, arg, tpl1.second->byClass.dflt);
                    }
                    else
                    {
                        GatherPackedTypes(count, arg, tpl.second->byClass.dflt);
                    }
                }
            }
        }
    }
}
void GatherPackedTypes(int* count, SYMBOL** arg, TYPE* tp)
{
    if (tp)
    {
        if (tp->type == bt_typedef && tp->sp->templateParams)
        {
            GatherTemplateParams(count, arg, tp->sp->templateParams);
        }
        else if (basetype(tp)->type == bt_templateselector)
        {
            for (auto&& tsl : *basetype(tp)->sp->sb->templateSelector)
            {
                if (tsl.templateParams)
                {
                    GatherTemplateParams(count, arg, tsl.templateParams);
                }
            }
        }
        else if (isstructured(tp))
        {
            if (basetype(tp)->sp->sb->templateLevel)
            {
                GatherTemplateParams(count, arg, basetype(tp)->sp->templateParams);
            }
        }
    }
}
void GatherPackedVars(int* count, SYMBOL** arg, EXPRESSION* packedExp)
{
    if (!packedExp)
        return;
    GatherPackedVars(count, arg, packedExp->left);
    GatherPackedVars(count, arg, packedExp->right);
    if (packedExp->type == en_auto && packedExp->v.sp->packed)
    {
        arg[(*count)++] = packedExp->v.sp;
        NormalizePacked(packedExp->v.sp->tp);
    }
    else if (packedExp->type == en_global && packedExp->v.sp->sb->parentClass)
    {
        // undefined
        SYMBOL* spx = packedExp->v.sp->sb->parentClass;
        while (spx)
        {
            std::list<TEMPLATEPARAMPAIR>* tpl = spx->templateParams;
            GatherTemplateParams(count, arg, spx->templateParams);
            spx = spx->sb->parentClass;
        }
    }
    else if (packedExp->type == en_func)
    {
        GatherTemplateParams(count, arg, packedExp->v.func->templateParams);
        INITLIST* lst;
        if (packedExp->v.func->arguments)
            for (auto lst : *packedExp->v.func->arguments)
                GatherPackedVars(count, arg, lst->exp);
    }
    else if (packedExp->type == en_templateselector)
    {
        for (auto&& tsl : *packedExp->v.templateSelector)
        {
            if (tsl.templateParams)
            {
                GatherTemplateParams(count, arg, tsl.templateParams);
            }
        }
    }
    else if (packedExp->type == en_templateparam)
    {
        arg[(*count)++] = packedExp->v.sp;
        NormalizePacked(packedExp->v.sp->tp);
    }
    else if (packedExp->type == en_construct)
    {
        GatherPackedTypes(count, arg, packedExp->v.construct.tp);
    }
}
EXPRESSION* ReplicatePackedVars(int count, SYMBOL** arg, EXPRESSION* packedExp, int index);
    TYPE* ReplicatePackedTypes(int count, SYMBOL** arg, TYPE* tp, int index);
std::list<TEMPLATEPARAMPAIR>* ReplicateTemplateParams(int count, SYMBOL** arg, std::list<TEMPLATEPARAMPAIR>* tplx, int index)
{
    if (tplx)
    {
        std::list<TEMPLATEPARAMPAIR>* rv = templateParamPairListFactory.CreateList();
        for (auto&& tpl : *tplx)
        {
            rv->push_back(TEMPLATEPARAMPAIR{ nullptr, nullptr });
            if (tpl.second->packed && tpl.first)
            {
                auto tparam = Allocate<TEMPLATEPARAM>();
                rv->back().second = tparam;
                tparam->packed = false;
                for (int j = 0; j < count; j++)
                    if (!strcmp(arg[j]->name, tpl.first->name))
                    {
                        auto ittpl1 = arg[j]->tp->templateParam->second->byPack.pack->begin();
                        auto ittpl1e = arg[j]->tp->templateParam->second->byPack.pack->end();
                        for (int k = 0; k < index && ittpl1 != ittpl1e; k++, ++ittpl1);
                        tparam->byClass.val = ittpl1->second->byClass.val ? ittpl1->second->byClass.val : ittpl1->second->byClass.dflt;
                        break;
                    }
            }
            else if (tpl.second->type == kw_int)
            {
                if (tpl.second->byNonType.dflt)
                {
                    auto tparam = Allocate<TEMPLATEPARAM>();
                    rv->back().second = tparam;
                    *tparam = *tpl.second;
                    tparam->packed = false;
                    tparam->byNonType.val = ReplicatePackedVars(count, arg, tpl.second->byNonType.dflt, index);
                }
            }
            else if (tpl.second->type == kw_typename)
            {
                if (tpl.second->byClass.dflt)
                {
                    auto tparam = Allocate<TEMPLATEPARAM>();
                    rv->back().second = tparam;
                    *tparam = *tpl.second;
                    tparam->packed = false;
                    tparam->byNonType.val = ReplicatePackedVars(count, arg, tpl.second->byNonType.dflt, index);
                }
            }
        }
        return rv;
    }
    return nullptr;
}
TYPE* ReplicatePackedTypes(int count, SYMBOL** arg, TYPE* tp, int index)
{
    if (tp)
    {
        if (tp->type == bt_templateselector)
        {
            tp = CopyType(tp);
            auto old = tp->sp->sb->templateSelector;
            auto tsl = tp->sp->sb->templateSelector = templateSelectorListFactory.CreateVector();
            for (auto&& x : *old)
            {
                tsl->push_back(x);
                if (x.templateParams)
                {
                    tsl->back().templateParams = ReplicateTemplateParams(count, arg, x.templateParams, index);
                }
            }
        }
        else if (isstructured(tp))
        {
            if (basetype(tp)->sp->sb->templateLevel)
            {
                tp = CopyType(tp, true, [count, arg, index](TYPE*& old, TYPE*& newx) {
                    if (old->rootType == old)
                        newx->sp->templateParams = ReplicateTemplateParams(count, arg, newx->sp->templateParams, index);
                });
            }
        }
    }
    return tp;
}
EXPRESSION* ReplicatePackedVars(int count, SYMBOL** arg, EXPRESSION* packedExp, int index)
{
    if (!packedExp)
        return packedExp;
    packedExp->left = ReplicatePackedVars(count, arg, packedExp->left, index);
    packedExp->right = ReplicatePackedVars(count, arg, packedExp->right, index);

    if (packedExp->type == en_auto && packedExp->v.sp->packed)
    {
        for (int j = 0; j < count; j++)
            if (!strcmp(arg[j]->name, packedExp->v.sp->name))
            {
                auto tpl = arg[j]->templateParams->front().second->byPack.pack->begin();
                auto tple = arg[j]->templateParams->front().second->byPack.pack->end();
                for (int k = 0; k < index && tpl != tple; k++, ++tpl);
                packedExp = tpl->second->byNonType.dflt;
                break;
            }
    }
    else
    {
        EXPRESSION* rv = Allocate<EXPRESSION>();
        *rv = *packedExp;
        packedExp = rv;
        if (packedExp->type == en_global && packedExp->v.sp->sb->parentClass)
        {
            // undefined
            SYMBOL** spx = &packedExp->v.sp;

            while (*spx)
            {
                auto next = Allocate<SYMBOL>();
                *next = **spx;
                if (next->templateParams)
                    next->templateParams = ReplicateTemplateParams(count, arg, next->templateParams, index);
                if ((*spx)->sb->parentClass)
                {
                    next->sb = Allocate<sym::_symbody>();
                    *next->sb = *(*spx)->sb;
                }
                spx = &(*spx)->sb->parentClass;
            }
        }
        else if (packedExp->type == en_func)
        {
            packedExp->v.func->templateParams = ReplicateTemplateParams(count, arg, packedExp->v.func->templateParams, index);
            if (packedExp->v.func->arguments)
            {
                std::list<INITLIST*> temp;
                for (auto old : *packedExp->v.func->arguments)
                {
                    auto lst = Allocate<INITLIST>();
                    *lst = *old;
                    lst->exp = ReplicatePackedVars(count, arg, lst->exp, index);
                    temp.push_back(lst);
                }
                *packedExp->v.func->arguments = temp;
            }
        }
        else if (packedExp->type == en_templateselector)
        {
            auto old = packedExp->v.templateSelector;
            auto tsl = packedExp->v.templateSelector = templateSelectorListFactory.CreateVector();
            for (auto&& x : *old)
            {
                tsl->push_back(x);
                if (x.templateParams)
                {
                    tsl->back().templateParams = ReplicateTemplateParams(count, arg, x.templateParams, index);
                }
            }
        }
        else if (packedExp->type == en_construct)
        {
            packedExp->v.construct.tp = ReplicatePackedTypes(count, arg, packedExp->v.construct.tp, index);
        }
    }
    return packedExp;
}
void ReplicatePackedExpression(EXPRESSION* pattern, int count, SYMBOL** arg, std::list<TEMPLATEPARAMPAIR>* dest)
{
    dest = templateParamPairListFactory.CreateList();
    int n = CountPacks(arg[0]->tp->templateParam->second->byPack.pack);
    for (int i = 0; i < n; i++)
    {
        dest->push_back(TEMPLATEPARAMPAIR{ nullptr, Allocate<TEMPLATEPARAM>() });
        dest->front().second->type = kw_int;
        dest->front().second->byNonType.dflt = ReplicatePackedVars(count, arg, pattern, i);
    }
}
int CountPacks(std::list<TEMPLATEPARAMPAIR>* packs)
{
    int rv = 0;
    if (packs)
    {
        return packs->size();
        /*
        std::stack<std::list<TEMPLATEPARAMPAIR>::iterator> tps;
        for (auto it = packs->begin(); it != packs->end();)
        {
            if (it->second->packed)
            {
                if (it->second->byPack.pack && it->second->byPack.pack->size())
                {
                    tps.push(it);
                    it = it->second->byPack.pack->begin();
                }
            }
            if (!it->second->packed)
            {
                rv++;
            }
            ++it;
            if (it == packs->end() && tps.size())
            {
                it = tps.top();
                tps.pop();
            }
        }
        */
    }
    return rv;
}
void expandPackedInitList(std::list<INITLIST*>** lptr, SYMBOL* funcsp, LEXLIST* start, EXPRESSION* packedExp)
{
    if (packedExp->type == en_templateparam)
    {
        if (packedExp->v.sp->tp->templateParam->second->packed)
        {
            if (packedExp->v.sp->tp->templateParam->second->byPack.pack)
            {
                if (!*lptr)
                    *lptr = initListListFactory.CreateList();
                for (auto&& t : *packedExp->v.sp->tp->templateParam->second->byPack.pack)
                {
                    auto il = Allocate<INITLIST>();
                    il->exp = t.second->byNonType.val;
                    il->tp = t.second->byNonType.tp;
                    (*lptr)->push_back(il);
                }
            }
        }
    }
    else
    {
        int oldPack = packIndex;
        int count = 0;
        SYMBOL* arg[200];
        GatherPackedVars(&count, arg, packedExp);
        expandingParams++;
        if (count)
        {
            if (arg[0]->sb && arg[0]->packed && arg[0]->sb->parent)
            {
                if (!*lptr)
                    *lptr = initListListFactory.CreateList();
                auto it = basetype(arg[0]->sb->parent->tp)->syms->begin();
                auto itend = basetype(arg[0]->sb->parent->tp)->syms->end();
                for (; it != itend && (*it) != arg[0]; ++it);
                if (it != itend)
                {
                    while (it != itend)
                    {
                        SYMBOL* sym = *it;
                        INITLIST* p = Allocate<INITLIST>();
                        p->tp = sym->tp;
                        p->exp = varNode(en_auto, sym);
                        if (isref(p->tp))
                        {
                            p->exp = exprNode(en_l_ref, p->exp, nullptr);
                            p->tp = basetype(p->tp)->btp;
                        }
                        if (!isstructured(p->tp))
                            deref(p->tp, &p->exp);
                        (*lptr)->push_back(p);
                        ++it;
                    }
                }
            }
            else
            {
                int i;
                int n = CountPacks(arg[0]->tp->templateParam->second->byPack.pack);
                if (n > 1 || !packedExp->v.func->arguments || !packedExp->v.func->arguments->size() || packedExp->v.func->arguments->front()->tp->type != bt_void)
                {
                    if (!*lptr)
                        *lptr = initListListFactory.CreateList();

                    for (i = 0; i < n; i++)
                    {
                        INITLIST* p = Allocate<INITLIST>();
                        LEXLIST* lex = SetAlternateLex(start);
                        packIndex = i;
                        expression_assign(lex, funcsp, nullptr, &p->tp, &p->exp, nullptr, _F_PACKABLE);
                        ConstExprPatch(&p->exp);
                        SetAlternateLex(nullptr);
                        if (p->tp->type != bt_void)
                            if (p->tp)
                            {
                                (*lptr)->push_back(p);
                            }
                    }
                }
            }
        }
        expandingParams--;
        packIndex = oldPack;
    }
}
static int GetBaseClassList(const char* name, SYMBOL* cls, std::list<BASECLASS*>* bases, std::list<BASECLASS*>& result)
{
    (void)cls;
    int n = 0;
    char str[1024];
    char* clslst[100];
    char *p = str, *c;
    int ccount = 0;
    strcpy(str, name);
    while ((c = (char*)strstr(p, "::")))
    {
        clslst[n++] = p;
        p = c;
        *p = 0;
        p += 2;
    }
    clslst[n++] = p;

    if (bases)
        for (auto bc : *bases)
            if (!bc->isvirtual)
            {
                SYMBOL* parent = bc->cls;
                int i;
                if (parent->tp->type == bt_typedef)
                    parent = basetype(parent->tp)->sp;
                for (i = n - 1; i >= 0 && parent;
                     i--, parent = parent->sb->parentClass ? parent->sb->parentClass : parent->sb->parentNameSpace)
                    if (strcmp(parent->name, clslst[i]))
                        break;
                if (i < 0 || (i == 0 && parent == nullptr && clslst[0][0] == '\0'))
                {
                    ++ccount;
                    result.push_back(bc);
                }
            }
    return ccount;
}
static int GetVBaseClassList(const char* name, SYMBOL* cls, std::list<VBASEENTRY*>* vbases, std::list<VBASEENTRY*>& result)
{
    (void)cls;
    int n = 0;
    char str[1024];
    char* clslst[100];
    char *p = str, *c;
    int vcount = 0;
    strcpy(str, name);
    while ((c = (char*)strstr(p, "::")))
    {
        clslst[n++] = p;
        p = c;
        *p = 0;
        p += 2;
    }
    clslst[n++] = p;

    if (vbases)
    {
        for (auto vbase : *vbases)
        {
            SYMBOL* parent = vbase->cls;
            int i;
            if (parent->tp->type == bt_typedef)
                parent = basetype(parent->tp)->sp;
            for (i = n - 1; i >= 0 && parent;
                i--, parent = parent->sb->parentClass ? parent->sb->parentClass : parent->sb->parentNameSpace)
                if (strcmp(parent->name, clslst[i]))
                    break;
            if (i < 0 || (i == 0 && parent == nullptr && clslst[0][0] == '\0'))
            {
                ++vcount;
                result.push_back(vbase);
            }
        }
    }
    return vcount;
}
void expandPackedBaseClasses(SYMBOL* cls, SYMBOL* funcsp, std::list<MEMBERINITIALIZERS*>::iterator& init,
                             std::list<MEMBERINITIALIZERS*>::iterator& initend,
                             std::list<MEMBERINITIALIZERS*>* mi, std::list<BASECLASS*>* bc,
                                            std::list<VBASEENTRY*>* vbase)
{
    MEMBERINITIALIZERS* linit = *init;
    int basecount = 0, vbasecount = 0;
    std::list<BASECLASS*> baseEntries;
    std::list<VBASEENTRY*> vbaseEntries;
    basecount = GetBaseClassList(linit->name, cls, bc, baseEntries);
    if (!basecount)
        vbasecount = GetVBaseClassList(linit->name, cls, vbase, vbaseEntries);
    if (!basecount && !vbasecount)
    {
        // already evaluated to not having a base class
        ++init;
    }
    else if ((basecount && !baseEntries.front()->cls->sb->templateLevel) || (vbasecount && !vbaseEntries.front()->cls->sb->templateLevel))
    {
        ++init;
        errorsym(ERR_NOT_A_TEMPLATE, linit->sp);
    }
    else
    {
        LEXLIST* lex = SetAlternateLex(linit->initData);
        init = mi->erase(init);
        if (MATCHKW(lex, lt))
        {
            // at this point we've already created the independent base classes
            // but the initdata has the argument list, so get it out of the way
            // and also count the number of packs to see if it matches the number of templates..
            int n = -1;
            std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
            LEXLIST* arglex = GetTemplateArguments(lex, funcsp, linit->sp, &lst);
            std::deque<TYPE*> defaults;
            PushPopDefaults(defaults, funcsp->templateParams, true, true);
            std::stack <TEMPLATEPARAM*> stk; 
            if (funcsp->templateParams)
            {
                for (auto l : *funcsp->templateParams)
                {
                    if (l.second->packed)
                    {
                        if (l.second->byPack.pack)
                            for (auto i : *l.second->byPack.pack)
                                stk.push(i.second);
                    }
                    else if (l.second->type != kw_new)
                        stk.push(l.second);
                }
                while (!stk.empty())
                {
                    auto t = stk.top();
                    stk.pop();
                    t->byClass.dflt = t->byClass.val;
                }
            }
            SetAlternateLex(nullptr);
            if (lst)
            {
                for (auto&& pack : *lst)
                {
                    if (pack.second->packed)
                    {
                        int n1 = CountPacks(pack.second->byPack.pack);
                        if (n == -1)
                        {
                            n = n1;
                        }
                        else if (n != n1)
                        {
                            error(ERR_PACK_SPECIFIERS_SIZE_MISMATCH);
                            break;
                        }
                    }
                }
            }
            if (n > 0)
            {
                int i;
                // the presumption is that the number of packed vars will be the same as the number
                // of instantiated templates...
                //
                // I have also seen n be 0 during parsing...
                auto itb = baseEntries.begin();
                auto itv = vbaseEntries.begin();
                int oldArgumentNesting = argumentNesting;
                argumentNesting = -1;
                for (i = 0; i < n; i++)
                {
                    int oldPack = packIndex;
                    SYMBOL* baseSP = basecount ? (*itb)->cls : (*itv)->cls;
                    if (basecount)
                        ++itb;
                    else
                        ++itv;
                    MEMBERINITIALIZERS* added = Allocate<MEMBERINITIALIZERS>();
                    bool done = false;
                    lex = SetAlternateLex(arglex);
                    packIndex = i;
                    added->name = linit->name;
                    init = mi->insert(init, added);
                    ++init;
                    added->sp = baseSP;
                    if (MATCHKW(lex, openpa) && added->sp->tp->sp->sb->trivialCons)
                    {
                        lex = getsym();
                        if (MATCHKW(lex, closepa))
                        {
                            lex = getsym();
                            added->init = nullptr;
                            initInsert(&added->init, nullptr, nullptr, added->sp->sb->offset, false);
                            done = true;
                        }
                        else
                        {
                            lex = backupsym();
                        }
                    }
                    if (!done)
                    {
                        SYMBOL* sym = makeID(sc_member, added->sp->tp, nullptr, added->sp->name);
                        FUNCTIONCALL shim;
                        added->sp = sym;
                        shim.arguments = nullptr;
                        getMemberInitializers(lex, funcsp, &shim, MATCHKW(lex, openpa) ? closepa : end, false);
                        if (shim.arguments)
                        {
                            added->init = initListFactory.CreateList();
                            for (auto a : *shim.arguments)
                            {
                                auto xinit = Allocate<INITIALIZER>();
                                xinit->basetp = a->tp;
                                xinit->exp = a->exp;
                                added->init->push_back(xinit);
                            }
                        }
                    }
                    SetAlternateLex(nullptr);
                    packIndex = oldPack;
                }
                argumentNesting = oldArgumentNesting;
            }
            PushPopDefaults(defaults, funcsp->templateParams, true, false);
        }
        else
        {
            lex = SetAlternateLex(nullptr);
            SpecializationError((char*)nullptr);
        }
    }
}
void expandPackedMemberInitializers(SYMBOL* cls, SYMBOL* funcsp, std::list<TEMPLATEPARAMPAIR>* templatePack, std::list<MEMBERINITIALIZERS*>** p,
                                    LEXLIST* start, std::list<INITLIST*>* list)
{
    int n = CountPacks(templatePack);
    MEMBERINITIALIZERS* orig = (*p)->front();
    (*p)->pop_front();
    auto itp = (*p)->begin();
    if (n)
    {
        int count = 0;
        int i;
        int oldPack;
        SYMBOL* arg[300];
        if (templatePack)
        {
            for (auto&& pack : *templatePack)
            {
                if (!isstructured(pack.second->byClass.val))
                {
                    error(ERR_STRUCTURED_TYPE_EXPECTED_IN_PACKED_TEMPLATE_PARAMETER);
                    return;
                }
            }
        }
        if (list)
            for (auto init : *list)
                GatherPackedVars(&count, arg, init->exp);
        for (i = 0; i < count; i++)
        {
            if (CountPacks(arg[i]->tp->templateParam->second->byPack.pack) != n)
            {
                error(ERR_PACK_SPECIFIERS_SIZE_MISMATCH);
                break;
            }
        }
        oldPack = packIndex;
        auto ittpp = templatePack->begin();
        auto ittppe = templatePack->end();
        for (i = 0; i < n; i++)
        {
            LEXLIST* lex = SetAlternateLex(start);
            MEMBERINITIALIZERS* mi = Allocate<MEMBERINITIALIZERS>();
            TYPE* tp = ittpp->second->byClass.val;
            int offset = 0;
            int vcount = 0, ccount = 0;
            *mi = *orig;
            mi->sp = nullptr;
            packIndex = i;
            mi->name = ittpp->second->byClass.val->sp->name;
            if (cls->sb->baseClasses)
                for (auto bc : *cls->sb->baseClasses)
                    if (!strcmp(bc->cls->name, mi->name))
                    {
                        if (bc->isvirtual)
                            vcount++;
                        else
                            ccount++;
                        mi->sp = bc->cls;
                        offset = bc->offset;
                    }
            if ((ccount && vcount) || ccount > 1)
            {
                errorsym2(ERR_NOT_UNAMBIGUOUS_BASE, mi->sp, cls);
            }
            if (mi->sp && mi->sp == tp->sp)
            {
                bool done = false;
                mi->sp->sb->offset = offset;
                if (MATCHKW(lex, openpa) && mi->sp->tp->sp->sb->trivialCons)
                {
                    lex = getsym();
                    if (MATCHKW(lex, closepa))
                    {
                        lex = getsym();
                        mi->init = nullptr;
                        initInsert(&mi->init, nullptr, nullptr, mi->sp->sb->offset, false);
                        done = true;
                    }
                    else
                    {
                        lex = backupsym();
                    }
                }
                if (!done)
                {
                    SYMBOL* sym = makeID(sc_member, mi->sp->tp, nullptr, mi->sp->name);
                    FUNCTIONCALL shim;
                    mi->sp = sym;
                    lex = SetAlternateLex(mi->initData);
                    shim.arguments = nullptr;
                    getMemberInitializers(lex, funcsp, &shim, MATCHKW(lex, openpa) ? closepa : end, false);
                    SetAlternateLex(nullptr);
                    if (shim.arguments)
                    {
                        mi->init = initListFactory.CreateList();
                        for (auto a : *shim.arguments)
                        {
                            auto xinit = Allocate<INITIALIZER>();
                            xinit->basetp = a->tp;
                            xinit->exp = a->exp;
                            mi->init->push_back(xinit);
                        }
                    }
                }
            }
            else
            {
                mi->sp = nullptr;
            }
            if (mi->sp)
            {
                (*p)->insert(itp, mi);
                ++itp;
            }
            else
            {
                errorstrsym(ERR_NOT_A_MEMBER_OR_BASE_CLASS, mi->name, cls);
            }
            SetAlternateLex(nullptr);
        }
        packIndex = oldPack;
    }
}
static bool classOrEnumParam(SYMBOL* param)
{
    TYPE* tp = param->tp;
    if (isref(tp))
        tp = basetype(tp)->btp;
    tp = basetype(tp);
    return isstructured(tp) || tp->type == bt_enum || tp->type == bt_templateparam || tp->type == bt_templateselector ||
           tp->type == bt_templatedecltype;
}
void checkOperatorArgs(SYMBOL* sp, bool asFriend)
{
    TYPE* tp = sp->tp;
    if (isref(tp))
        tp = basetype(tp)->btp;
    if (!isfunction(tp))
    {
        char buf[256];
        if (sp->sb->castoperator)
        {
            strcpy(buf, "typedef()");
        }
        else
        {
            strcpy(buf, overloadXlateTab[sp->sb->operatorId]);
        }
        errorstr(ERR_OPERATOR_MUST_BE_FUNCTION, buf);
    }
    else
    {
        auto it = basetype(sp->tp)->syms->begin();
        auto itend = basetype(sp->tp)->syms->end();
        if (it == itend)
            return;
        if (!asFriend && getStructureDeclaration())  // nonstatic member
        {
            if (sp->sb->operatorId == CI_CAST)
            {
                // needs no argument
                SYMBOL* sym = *it;
                if (sym->tp->type != bt_void)
                {
                    errortype(ERR_OPERATOR_NEEDS_NO_PARAMETERS, basetype(sym->tp)->btp, nullptr);
                }
            }
            else
                switch ((enum e_kw)(sp->sb->operatorId - CI_NEW))
                {
                    SYMBOL* sym;
                    case plus:
                    case minus:
                    case star:
                    case andx:
                        // needs zero or one argument
                        if (basetype(sp->tp)->syms->size() > 1)
                        {
                            errorstr(ERR_OPERATOR_NEEDS_ZERO_OR_ONE_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                        }
                        if (basetype(sp->tp)->syms->size() == 0)
                        {
                            switch ((enum e_kw)(sp->sb->operatorId - CI_NEW))
                            {
                                case plus:
                                    sp->sb->operatorId = plus_unary;
                                    break;
                                case minus:
                                    sp->sb->operatorId = minus_unary;
                                    break;
                                case star:
                                    sp->sb->operatorId = star_unary;
                                    break;
                                case andx:
                                    sp->sb->operatorId = and_unary;
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    case notx:
                    case complx:
                        // needs no argument
                        sym = *it;
                        if (sym->tp->type != bt_void)
                        {
                            errorstr(ERR_OPERATOR_NEEDS_NO_PARAMETERS, overloadXlateTab[sp->sb->operatorId]);
                        }
                        break;
                    case divide:
                    case leftshift:
                    case rightshift:
                    case mod:
                    case eq:
                    case neq:
                    case lt:
                    case leq:
                    case gt:
                    case geq:
                    case land:
                    case lor:
                    case orx:
                    case uparrow:
                    case comma:
                    case pointstar:
                        // needs one argument
                        sym = *it;
                        if (sym->tp->type == bt_void || basetype(sp->tp)->syms->size() > 1)
                        {
                            errorstr(ERR_OPERATOR_NEEDS_ONE_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                        }
                        break;
                    case autoinc:
                    case autodec:
                        if (basetype(sp->tp)->syms->size() > 1)
                        {
                            errorstr(ERR_OPERATOR_NEEDS_ZERO_OR_ONE_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                        }
                        sym = *it;
                        if (sym->tp->type != bt_void && sym->tp->type != bt_int)
                        {
                            errorstr(ERR_OPERATOR_NEEDS_PARAMETER_OF_TYPE_INT, overloadXlateTab[sp->sb->operatorId]);
                        }
                        break;
                    case kw_new:
                    case kw_delete:
                    case kw_newa:  // new[]
                    case kw_dela:  // delete[]
                        break;
                    case assign:
                    case asplus:
                    case asminus:
                    case astimes:
                    case asdivide:
                    case asmod:
                    case asleftshift:
                    case asrightshift:
                    case asand:
                    case asor:
                    case asxor:
                        // needs one argument
                        sym = *it;
                        if (sym->tp->type == bt_void || basetype(sp->tp)->syms->size() > 1)
                        {
                            errorstr(ERR_OPERATOR_NEEDS_ONE_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                        }
                        break;

                    case openbr:
                        // needs one argument:
                        sym = *it;
                        if (sym->tp->type == bt_void || basetype(sp->tp)->syms->size() > 1)
                        {
                            errorstr(ERR_OPERATOR_NEEDS_ONE_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                        }
                        break;
                    case openpa:
                        // anything goes
                        break;
                    case pointsto:
                        // needs no arguments
                        sym = *it;
                        if (sym->tp->type != bt_void)
                        {
                            errorstr(ERR_OPERATOR_NEEDS_NO_PARAMETERS, overloadXlateTab[sp->sb->operatorId]);
                        }
                        break;
                    case quot:
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
                    errortype(ERR_OPERATOR_NONSTATIC, basetype(sp->tp)->btp, nullptr);
                }
                else if ((enum e_kw)(sp->sb->operatorId - CI_NEW) != kw_new &&
                         (enum e_kw)(sp->sb->operatorId - CI_NEW) != kw_delete &&
                         (enum e_kw)(sp->sb->operatorId - CI_NEW) != kw_newa &&
                         (enum e_kw)(sp->sb->operatorId - CI_NEW) != kw_dela)
                    {
                    errorstr(ERR_OPERATOR_NONSTATIC, overloadXlateTab[sp->sb->operatorId]);
                }
            }
        }
        else
        {
            switch ((enum e_kw)(sp->sb->operatorId - CI_NEW))
            {
                SYMBOL* sym;
                case plus:
                case minus:
                case star:
                case andx:
                case asplus:
                case asminus:
                case astimes:
                case asdivide:
                case asmod:
                case asleftshift:
                case asrightshift:
                case asand:
                case asor:
                case asxor:
                    // needs one or two arguments, one being class type
                    sym = (*it);
                    ++it;
                    if (sym->tp->type == bt_void || basetype(sp->tp)->syms->size() > 2)
                    {
                        errorstr(ERR_OPERATOR_NEEDS_ONE_OR_TWO_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                    }
                    else if (!classOrEnumParam(sym) && (basetype(sp->tp)->syms->size() == 1 || !classOrEnumParam((*it))))
                    {
                        if (!templateNestingCount)
                            errorstr(ERR_OPERATOR_NEEDS_A_CLASS_OR_ENUMERATION_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                    }
                    break;
                case notx:
                case complx:
                    // needs one arg of class or enum type
                    sym = *it;
                    if (sym->tp->type == bt_void || basetype(sp->tp)->syms->size() != 1)
                    {
                        errorstr(ERR_OPERATOR_NEEDS_ONE_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                    }
                    else if (!classOrEnumParam(sym))
                    {
                        if (!templateNestingCount)
                            errorstr(ERR_OPERATOR_NEEDS_A_CLASS_OR_ENUMERATION_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                    }
                    break;

                case divide:
                case leftshift:
                case rightshift:
                case mod:
                case eq:
                case neq:
                case lt:
                case leq:
                case gt:
                case geq:
                case land:
                case lor:
                case orx:
                case uparrow:
                case comma:
                    // needs two args, one of class or enum type
                    if (basetype(sp->tp)->syms->size() != 2)
                    {
                        errorstr(ERR_OPERATOR_NEEDS_TWO_PARAMETERS, overloadXlateTab[sp->sb->operatorId]);
                    }
                    else if (!classOrEnumParam(*it) && !classOrEnumParam(*++it))
                    {
                        if (!templateNestingCount)
                            errorstr(ERR_OPERATOR_NEEDS_A_CLASS_OR_ENUMERATION_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                    }
                    break;
                case autoinc:
                case autodec:
                    // needs one or two args, first of class or enum type
                    // if second is present int type
                    sym = (*it);
                    if (sym->tp->type == bt_void || basetype(sp->tp)->syms->size() > 2)
                    {
                        errorstr(ERR_OPERATOR_NEEDS_ONE_OR_TWO_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                    }
                    else if (!classOrEnumParam(sym))
                    {
                        if (!templateNestingCount)
                            errorstr(ERR_OPERATOR_NEEDS_A_CLASS_OR_ENUMERATION_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                    }
                    if (basetype(sp->tp)->syms->size() == 2)
                    {
                        sym = *++it;
                        if (sym->tp->type != bt_int)
                        {
                            errorstr(ERR_OPERATOR_NEEDS_SECOND_PARAMETER_OF_TYPE_INT, overloadXlateTab[sp->sb->operatorId]);
                        }
                    }
                    break;
                case quot:
                    if (basetype(sp->tp)->syms->size() > 2)
                    {
                        errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                    }
                    else if (basetype(sp->tp)->syms->size() == 2)
                    {
                        // two args
                        TYPE* tpl = (*it)->tp;
                        TYPE* tpr = (*++it)->tp;
                        if (!isunsigned(tpr) || !ispointer(tpl))
                        {
                            errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                        }
                        else
                        {
                            tpl = basetype(tpl)->btp;
                            tpr = basetype(tpl);
                            if (!isconst(tpl) || (tpr->type != bt_char && tpr->type != bt_wchar_t && tpr->type != bt_char16_t &&
                                                    tpr->type != bt_char32_t))
                            {
                                errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                            }
                        }
                    }
                    else
                    {
                        // one arg
                        TYPE* tp = (*it)->tp;
                        if ((!ispointer(tp) || !isconst(basetype(tp)->btp) || basetype(basetype(tp)->btp)->type != bt_char) &&
                            tp->type != bt_unsigned_long_long && tp->type != bt_long_double && tp->type != bt_char &&
                            tp->type != bt_wchar_t && tp->type != bt_char16_t && tp->type != bt_char32_t)
                        {
                            errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                        }
                    }
                    break;
                case kw_new:
                case kw_newa:
                    if (basetype(sp->tp)->syms->size() > 0)
                    {
                        // any number of args, but first must be a size
                        TYPE* tp = (*it)->tp;
                        if (!isint(tp))
                            errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                    }
                    else
                    {
                        errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                    }
                    break;
                case kw_delete:
                case kw_dela:
                    if (basetype(sp->tp)->syms->size() > 0)
                    {
                        // one or more args, first must be a pointer
                        TYPE* tp = (*it)->tp;
                        if (!ispointer(tp))
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
                        errortype(ERR_OPERATOR_MUST_BE_NONSTATIC, basetype(sp->tp)->btp, nullptr);
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
LEXLIST* handleStaticAssert(LEXLIST* lex)
{
    if (!needkw(&lex, openpa))
    {
        errskim(&lex, skim_closepa);
        skip(&lex, closepa);
    }
    else
    {
        bool v = true;
        char buf[5000];
        TYPE* tp;
        EXPRESSION *expr = nullptr, *expr2 = nullptr;
        inConstantExpression++;
        lex = expression_no_comma(lex, nullptr, nullptr, &tp, &expr, nullptr, 0);
        expr2 = Allocate<EXPRESSION>();
        expr2->type = en_x_bool;
        expr2->left = expr->type == en_select ? expr->left : expr;
        optimize_for_constants(&expr2);
        inConstantExpression--;
        if (!isarithmeticconst(expr2) && !templateNestingCount)
            error(ERR_CONSTANT_VALUE_EXPECTED);
        v = expr2->v.i;

        if (MATCHKW(lex, comma))
        {
            lex = getsym();
            if (lex->data->type != l_astr)
            {
                error(ERR_NEEDSTRING);
            }
            else
            {
                int i, pos = 0;
                while (lex->data->type == l_astr)
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
            strcpy(buf, "(unspecified)");
        }
        if (!needkw(&lex, closepa))
        {
            errskim(&lex, skim_closepa);
            skip(&lex, closepa);
        }
        else if (!v && (!templateNestingCount))  // || instantiatingTemplate))
        {
            errorstr(ERR_STATIC_ASSERT, buf);
        }
    }
    return lex;
}
LEXLIST* insertNamespace(LEXLIST* lex, enum e_lk linkage, enum e_sc storage_class, bool* linked)
{
    bool anon = false;
    char buf[256], *p;
    SYMBOL* sym;
    Optimizer::LIST* list;
    *linked = false;
    if (ISID(lex))
    {
        strcpy(buf, lex->data->value.s.a);
        lex = getsym();
        if (MATCHKW(lex, assign))
        {
            lex = getsym();
            if (ISID(lex))
            {
                char buf1[512];
                strcpy(buf1, lex->data->value.s.a);
                lex = nestedSearch(lex, &sym, nullptr, nullptr, nullptr, nullptr, false, sc_global, true, false);
                if (sym)
                {
                    if (sym->sb->storage_class != sc_namespace)
                    {
                        errorsym(ERR_NOT_A_NAMESPACE, sym);
                    }
                    else
                    {
                        SYMBOL* src = sym;
                        SYMBOL* sym = nullptr;
                        TYPE* tp;
                        SYMLIST** p;
                        if (storage_class == sc_auto)
                            sym = localNameSpace->front()->syms->Lookup(buf);
                        else
                            sym = globalNameSpace->front()->syms->Lookup(buf);
                        if (sym)
                        {
                            // already exists, bug check it
                            if (sym->sb->storage_class == sc_namespacealias && sym->sb->nameSpaceValues->front()->origname == src)
                            {
                                if (linkage == lk_inline)
                                {
                                    error(ERR_INLINE_NOT_ALLOWED);
                                }
                                lex = getsym();
                                return lex;
                            }
                        }
                        tp = MakeType(bt_void);
                        sym = makeID(sc_namespacealias, tp, nullptr, litlate(buf));
                        if (nameSpaceList.size())
                        {
                            sym->sb->parentNameSpace = nameSpaceList.front();
                        }
                        SetLinkerNames(sym, lk_none);
                        if (storage_class == sc_auto)
                        {
                            localNameSpace->front()->syms->Add(sym);
                            localNameSpace->front()->tags->Add(sym);
                        }
                        else
                        {
                            globalNameSpace->front()->syms->Add(sym);
                            globalNameSpace->front()->tags->Add(sym);
                        }
                        sym->sb->nameSpaceValues = namespaceValueDataListFactory.CreateList();
                        *sym->sb->nameSpaceValues = *src->sb->nameSpaceValues;
                        *sym->sb->nameSpaceValues->begin() = Allocate<NAMESPACEVALUEDATA>();
                        **sym->sb->nameSpaceValues->begin() = **src->sb->nameSpaceValues->begin();
                        sym->sb->nameSpaceValues->front()->name = sym;  // this is to rename it with the alias e.g. for errors
                    }
                }
                if (linkage == lk_inline)
                {
                    error(ERR_INLINE_NOT_ALLOWED);
                }
                lex = getsym();
            }
            else
            {
                error(ERR_EXPECTED_NAMESPACE_NAME);
            }
            return lex;
        }
    }
    else
    {
        anon = true;
        if (!anonymousNameSpaceName[0])
        {
            p = (char*)strrchr(infile, '\\');
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

            sprintf(anonymousNameSpaceName, "__%s__%08x", p, Utils::CRC32((unsigned char*)infile, strlen(infile)));
            while ((p = (char*)strchr(anonymousNameSpaceName, '.')) != 0)
                *p = '_';
        }
        strcpy(buf, anonymousNameSpaceName);
    }
    if (storage_class != sc_global)
    {
        error(ERR_NO_NAMESPACE_IN_FUNCTION);
    }
    SYMBOL *sp = globalNameSpace->front()->syms->Lookup(buf);
    if (!sp)
    {
        sym = makeID(sc_namespace, MakeType(bt_void), nullptr, litlate(buf));
        sym->sb->nameSpaceValues = namespaceValueDataListFactory.CreateList();
        *sym->sb->nameSpaceValues = *globalNameSpace;
        sym->sb->nameSpaceValues->push_front(Allocate<NAMESPACEVALUEDATA>());
        sym->sb->nameSpaceValues->front()->syms = symbols.CreateSymbolTable();
        sym->sb->nameSpaceValues->front()->tags = symbols.CreateSymbolTable();
        sym->sb->nameSpaceValues->front()->origname = sym;
        sym->sb->nameSpaceValues->front()->name = sym;
        sym->sb->parentNameSpace = globalNameSpace->front()->name;
        sym->sb->attribs.inheritable.linkage = linkage;
        if (nameSpaceList.size())
        {
            sym->sb->parentNameSpace = nameSpaceList.front();
        }
        SetLinkerNames(sym, lk_none);
        globalNameSpace->front()->syms->Add(sym);
        globalNameSpace->front()->tags->Add(sym);
        if (anon || linkage == lk_inline)
        {
            // plop in a using directive for the anonymous namespace we are declaring
            if (linkage == lk_inline)
            {
                if (!globalNameSpace->front()->inlineDirectives)
                    globalNameSpace->front()->inlineDirectives = symListFactory.CreateList();
                globalNameSpace->front()->inlineDirectives->push_front(sym);
            }
            else
            {
                if (!globalNameSpace->front()->usingDirectives)
                    globalNameSpace->front()->usingDirectives = symListFactory.CreateList();
                globalNameSpace->front()->usingDirectives->push_front(sym);
            }
        }
    }
    else
    {
        sym = sp;
        if (sym->sb->storage_class != sc_namespace)
        {
            errorsym(ERR_NOT_A_NAMESPACE, sym);
            return lex;
        }
        if (linkage == lk_inline)
            if (sym->sb->attribs.inheritable.linkage != lk_inline)
                errorsym(ERR_NAMESPACE_NOT_INLINE, sym);
    }
    sym->sb->value.i++;

    nameSpaceList.push_front(sym);

    globalNameSpace->push_front(sym->sb->nameSpaceValues->front());

    *linked = true;
    return lex;
}
void unvisitUsingDirectives(NAMESPACEVALUEDATA* v)
{
    if (v->usingDirectives)
    {
        for (auto sym : *v->usingDirectives)
        {
            sym->sb->visited = false;
            unvisitUsingDirectives(sym->sb->nameSpaceValues->front());
        }
    }
    if (v->inlineDirectives)
    {
        for (auto sym : *v->inlineDirectives)
        {
            sym->sb->visited = false;
            unvisitUsingDirectives(sym->sb->nameSpaceValues->front());
        }
    }
}
static void InsertTag(SYMBOL* sym, enum e_sc storage_class, bool allowDups)
{
    SymbolTable<SYMBOL>* table;
    SYMBOL* ssp = getStructureDeclaration();
    SYMBOL* sp1 = nullptr;
    if (ssp && (storage_class == sc_member || storage_class == sc_mutable || storage_class == sc_type))
    {
        table = ssp->tp->tags;
    }
    else if (storage_class == sc_auto || storage_class == sc_register || storage_class == sc_parameter ||
             storage_class == sc_localstatic)
        table = localNameSpace->front()->tags;
    else
        table = globalNameSpace->front()->tags;
    if (allowDups)
        sp1 = search(table, sym->name);
    if (!allowDups || !sp1 || (sym != sp1 && sym->sb->mainsym && sym->sb->mainsym != sp1->sb->mainsym))
        table->Add(sym);
}
LEXLIST* insertUsing(LEXLIST* lex, SYMBOL** sp_out, enum e_ac access, enum e_sc storage_class, bool inTemplate, bool hasAttributes)
{
    SYMBOL* sp;
    if (MATCHKW(lex, kw_namespace))
    {
        lex = getsym();
        if (ISID(lex))
        {
            // by spec using directives match the current state of
            // the namespace at all times... so we cache pointers to
            // related namespaces
            lex = nestedSearch(lex, &sp, nullptr, nullptr, nullptr, nullptr, false, sc_global, true, false);
            if (sp)
            {
                if (sp->sb->storage_class != sc_namespace && sp->sb->storage_class != sc_namespacealias)
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
        if (MATCHKW(lex, kw_typename))
        {
            isTypename = true;
            lex = getsym();
        }

        if (hasAttributes)
            error(ERR_NO_ATTRIBUTE_SPECIFIERS_HERE);
        if (!isTypename && ISID(lex))
        {
            LEXLIST* idsym = lex;
            lex = getsym();
            attributes oldAttribs = basisAttribs;
            basisAttribs = {0};
            ParseAttributeSpecifiers(&lex, nullptr, true);
            if (MATCHKW(lex, assign))
            {
                TYPE* tp = nullptr;
                SYMBOL* sp;
                lex = getsym();
                std::list<TEMPLATEPARAMPAIR>* lst = nullptr;
                bool pulledtypename = false;
                if (MATCHKW(lex, kw_typename))
                {
                    pulledtypename = true;
                    lex = getsym();
                }
                if (inTemplate && (ISID(lex) || MATCHKW(lex, classsel) || MATCHKW(lex, kw_typename)))
                {
                    SYMBOL *sym = nullptr, *strsym = nullptr;
                    std::list<NAMESPACEVALUEDATA*>* ns = nullptr;
                    bool throughClass = false;
                    parsingTrailingReturnOrUsing++;
                    lex = id_expression(lex, nullptr, &sym, &strsym, nullptr, nullptr, false, false, nullptr);
                    if (sym)
                    {
                        tp = sym->tp;
                        lex = getsym();
                        if (MATCHKW(lex, lt))
                        {
                            lex = GetTemplateArguments(lex, nullptr, sym, &lst);
                        }
                    }
                    else if (strsym && strsym->tp->type == bt_templateselector)
                    {
                        tp = strsym->tp;
                        lex = getsym();
                        if (MATCHKW(lex, lt))
                        {
                            lex = GetTemplateArguments(lex, (*strsym->tp->sp->sb->templateSelector)[1].sp, sym, &lst);
                        }
                    }
                    parsingTrailingReturnOrUsing--;
                    enum e_lk linkage = lk_none, linkage2 = lk_none, linkage3 = lk_none;
                    bool defd = false;
                    SYMBOL* sp = nullptr;
                    bool notype = false;
                    bool oldTemplateType = inTemplateType;

                    lex = getBeforeType(lex, nullptr, &tp, &sp, nullptr, nullptr, false, storage_class, &linkage, &linkage2,
                                        &linkage3, nullptr, false, false, true, false); /* fixme at file scope init */
                }
                else
                {
                    if (pulledtypename)
                        lex = backupsym();
                    parsingUsing++;
                    lex = get_type_id(lex, &tp, nullptr, sc_cast, false, true, true);
                    parsingUsing--;
                }
                if (!tp)
                {
                    tp = &stdint;
                }
                checkauto(tp, ERR_AUTO_NOT_ALLOWED_IN_USING_STATEMENT);
                sp = makeID(sc_typedef, tp, nullptr, litlate(idsym->data->value.s.a));
                sp->sb->attribs = basisAttribs;
                sp->tp = MakeType(bt_typedef, tp);
                sp->tp->sp = sp;
                sp->sb->access = access;
                if (inTemplate)
                {
                    sp->sb->templateLevel = templateNestingCount;
                    sp->templateParams = TemplateGetParams(sp);
                    if (isstructured(tp) || basetype(tp)->type == bt_templateselector)
                        sp->sb->typeAlias = lst;
                }
                else if (!templateNestingCount)
                {
                    sp->tp = PerformDeferredInitialization(sp->tp, nullptr);
                }
                if (storage_class == sc_member)
                    sp->sb->parentClass = getStructureDeclaration();
                SetLinkerNames(sp, lk_cdecl);
                InsertSymbol(sp, storage_class, lk_cdecl, false);
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
        SYMBOL* strsym = nullptr;
        lex = nestedSearch(lex, &sp, &strsym, nullptr, nullptr, nullptr, false, sc_global, true, false);
        if (sp)
        {
            if (sp->sb->mainsym && sp->sb->mainsym == strsym)
                sp = search(strsym->tp->syms, overloadNameTab[CI_CONSTRUCTOR]);
            if (sp)
            {
                if (!templateNestingCount)
                {
                    if (sp->sb->storage_class == sc_overloads)
                    {
                        for (auto sp2 : * sp->tp->syms)
                        {
                            SYMBOL *ssp = getStructureDeclaration(), *ssp1;
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
                        if (isTypename)
                            error(ERR_TYPE_NAME_EXPECTED);
                    }
                    else
                    {
                        SYMBOL *ssp = getStructureDeclaration(), *ssp1;
                        SYMBOL* sp1 = CopySymbol(sp);
                        sp1->sb->wasUsing = true;
                        sp1->sb->mainsym = sp;
                        sp1->sb->access = access;
                        ssp1 = sp1->sb->parentClass;
                        if (ssp && ismember(sp1))
                            sp1->sb->parentClass = ssp;
                        if (isTypename && !istype(sp))
                            error(ERR_TYPE_NAME_EXPECTED);
                        if (istype(sp))
                            InsertTag(sp1, storage_class, true);
                        else
                            InsertSymbol(sp1, storage_class, lk_cdecl, true);
                        sp1->sb->parentClass = ssp1;
                    }
                }
            }
            lex = getsym();
        }
    }
    return lex;
}
static void balancedAttributeParameter(LEXLIST** lex)
{
    enum e_kw start = KW(*lex);
    enum e_kw endp = (enum e_kw) - 1;
    *lex = getsym();
    switch (start)
    {
        case openpa:
            endp = closepa;
            break;
        case begin:
            endp = end;
            break;
        case openbr:
            endp = closebr;
            break;
        default:
            break;
    }
    if (endp != (enum e_kw) - 1)
    {
        while (*lex && !MATCHKW(*lex, endp))
            balancedAttributeParameter(lex);
        needkw(lex, endp);
    }
}
TYPE* AttributeFinish(SYMBOL* sym, TYPE* tp)
{
    sym->sb->attribs = basisAttribs;
    basetype(tp)->alignment = sym->sb->attribs.inheritable.structAlign;
    // should come first to overwrite all other attributes
    if (sym->sb->attribs.uninheritable.copyFrom)
    {
        if (isfunction(sym->tp) == isfunction(sym->sb->attribs.uninheritable.copyFrom->tp) &&
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
        if (isarithmetic(tp))
        {
            int n = sym->sb->attribs.inheritable.vectorSize % tp->size;
            int m = sym->sb->attribs.inheritable.vectorSize / tp->size;
            if (n || m > 0x10000 || (m & (m - 1)) != 0)
                error(ERR_INVALID_VECTOR_SIZE);
            tp = MakeType(bt_pointer, tp);
            tp->size = sym->sb->attribs.inheritable.vectorSize;
            tp->array = true;
        }
        else
        {
            error(ERR_INVALID_VECTOR_TYPE);
        }
    }
    if (sym->sb->attribs.inheritable.cleanup && sym->sb->storage_class == sc_auto)
    {
        FUNCTIONCALL* fc = Allocate<FUNCTIONCALL>();
        fc->arguments = initListListFactory.CreateList();
        auto arg = Allocate<INITLIST>();
        arg->tp = &stdpointer;
        arg->exp = varNode(en_auto, sym);
        fc->arguments->push_back(arg);
        fc->ascall = true;
        fc->functp = sym->sb->attribs.inheritable.cleanup->tp;
        fc->fcall = varNode(en_pc, sym->sb->attribs.inheritable.cleanup);
        fc->sp = sym->sb->attribs.inheritable.cleanup;
        EXPRESSION* expl = exprNode(en_func, nullptr, nullptr);
        expl->v.func = fc;
        initInsert(&sym->sb->dest, sym->tp, expl, 0, true);
    }
    return tp;
}
static const std::unordered_map<std::string, int> gccStyleAttribNames = {
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
};
void ParseOut__attribute__(LEXLIST** lex, SYMBOL* funcsp)
{
    if (MATCHKW(*lex, kw__attribute))
    {
        *lex = getsym();
        if (needkw(lex, openpa))
        {
            if (needkw(lex, openpa))
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
                                if (MATCHKW(*lex, openpa))
                                {
                                    *lex = getsym();
                                    if ((*lex)->data->type == l_astr)
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
                                    needkw(lex, closepa);
                                }
                                break;
                            case 2:  // aligned
                                if (MATCHKW(*lex, openpa))
                                {
                                    *lex = getsym();
                                    TYPE* tp = nullptr;
                                    EXPRESSION* exp = nullptr;

                                    *lex = optimized_expression(*lex, funcsp, nullptr, &tp, &exp, false);
                                    if (!tp || !isint(tp))
                                        error(ERR_NEED_INTEGER_TYPE);
                                    else if (!isintconst(exp))
                                        error(ERR_CONSTANT_VALUE_EXPECTED);
                                    basisAttribs.inheritable.structAlign = exp->v.i;
                                    basisAttribs.inheritable.alignedAttribute = true;
                                    needkw(lex, closepa);

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
                                if (needkw(lex, openpa))
                                {
                                    TYPE* tp = nullptr;
                                    EXPRESSION* exp = nullptr;

                                    *lex = optimized_expression(*lex, funcsp, nullptr, &tp, &exp, false);
                                    if (!tp || !isint(tp))
                                        error(ERR_NEED_INTEGER_TYPE);
                                    else if (!isintconst(exp))
                                        error(ERR_CONSTANT_VALUE_EXPECTED);
                                    basisAttribs.inheritable.warnAlign = exp->v.i;
                                    needkw(lex, closepa);

                                    if (basisAttribs.inheritable.warnAlign > 0x10000 ||
                                        (basisAttribs.inheritable.warnAlign & (basisAttribs.inheritable.warnAlign - 1)) != 0)
                                        error(ERR_INVALID_ALIGNMENT);
                                }
                                break;
                            case 4:  // alloc_size // doesn't restrict to numbers but maybe should?
                                if (needkw(lex, openpa))
                                {
                                    errskim(lex, skim_comma);
                                    if (MATCHKW(*lex, comma))
                                    {
                                        *lex = getsym();
                                        errskim(lex, skim_closepa);
                                    }
                                    needkw(lex, closepa);
                                }
                                break;
                            case 5:  // cleanup - needs work, should be in the C++ exception table for the function...
                                if (MATCHKW(*lex, openpa))
                                {
                                    *lex = getsym();
                                    if (ISID(*lex))
                                    {
                                        SYMBOL* sym = gsearch((*lex)->data->value.s.a);
                                        if (sym)
                                        {
                                            if (sym->tp->type == bt_aggregate)
                                                if (basetype(sym->tp)->syms->size() == 1)
                                                    sym = basetype(sym->tp)->syms->front();
                                            if (isfunction(sym->tp) && isvoid(basetype(sym->tp)->btp))
                                            {
                                                if (basetype(sym->tp)->syms->size() == 0)
                                                {
                                                    basisAttribs.inheritable.cleanup = sym;
                                                }
                                                else if (basetype(sym->tp)->syms->size() == 1)
                                                {
                                                    auto sp = basetype(sym->tp)->syms->front();
                                                    if (isvoid(sp->tp) || isvoidptr(sp->tp))
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
                                    needkw(lex, closepa);
                                }
                                break;
                            case 8:  // copy
                                if (MATCHKW(*lex, openpa))
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
                                    needkw(lex, closepa);
                                }
                                break;
                            case 9:  // deprecated
                                basisAttribs.uninheritable.deprecationText = (const char*)-1;
                                if (MATCHKW(*lex, openpa))
                                {
                                    *lex = getsym();
                                    if ((*lex)->data->type == l_astr)
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
                                    needkw(lex, closepa);
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
                                if (needkw(lex, openpa))
                                {
                                    TYPE* tp = nullptr;
                                    EXPRESSION* exp = nullptr;

                                    *lex = optimized_expression(*lex, funcsp, nullptr, &tp, &exp, false);
                                    if (!tp || !isint(tp))
                                        error(ERR_NEED_INTEGER_TYPE);
                                    else if (!isintconst(exp))
                                        error(ERR_CONSTANT_VALUE_EXPECTED);
                                    basisAttribs.inheritable.vectorSize = exp->v.i;
                                    needkw(lex, closepa);
                                }
                                break;
                            case 19:  // dllimport
                                if (basisAttribs.inheritable.linkage2 != lk_none)
                                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                basisAttribs.inheritable.linkage2 = lk_import;
                                break;
                            case 20:  // dllexport
                                if (basisAttribs.inheritable.linkage2 != lk_none)
                                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                basisAttribs.inheritable.linkage2 = lk_export;
                                break;
                            case 23:  // zstring
                                basisAttribs.inheritable.zstring = true;
                                break;
                            case 24:
                                if (basisAttribs.inheritable.linkage3 != lk_none)
                                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                basisAttribs.inheritable.linkage3 = lk_noreturn;
                                break;
                            case 25:  // stdcall
                                if (basisAttribs.inheritable.linkage != lk_none)
                                    error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                basisAttribs.inheritable.linkage = lk_stdcall;
                                break;
                            case 26:  // always inline
                                basisAttribs.inheritable.isInline = true;
                                break;
                            case 27:  // format
                                needkw(lex, openpa);
                                while (*lex && !MATCHKW(*lex, closepa))
                                    *lex = getsym();
                                if (lex)
                                    *lex = getsym();
                                break;
                            case 28:  // internal_linkage
                                basisAttribs.inheritable.linkage2 = lk_internal;
                                break;
                            case 29:  // exclude_from_explicit_instantiation
                                basisAttribs.inheritable.excludeFromExplicitInstantiation = true;
                                break;
                        }
                    }
                }
                needkw(lex, closepa);
                needkw(lex, closepa);
            }
            else
            {
                errskim(lex, skim_closepa);
                skip(lex, closepa);
                error(ERR_IGNORING__ATTRIBUTE);
            }
        }
    }
}
static const std::unordered_map<std::string, int> occCPPStyleAttribNames = {
    {"zstring", 23},  // non-gcc, added to support nonstring
};
static const std::unordered_map<std::string, int> clangCPPStyleAttribNames = {
    {"internal_linkage", 28},
    {"exclude_from_explicit_instantiation", 29},
};
static const std::unordered_map<std::string, int> gccCPPStyleAttribNames = {
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
bool ParseAttributeSpecifiers(LEXLIST** lex, SYMBOL* funcsp, bool always)
{
    (void)always;
    bool rv = false;
    if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.prm_c1x)
    {
        while (MATCHKW(*lex, kw_alignas) || MATCHKW(*lex, kw__attribute) ||
               ((Optimizer::cparams.prm_cplusplus || Optimizer::cparams.prm_c2x) && MATCHKW(*lex, openbr)))
        {
            if (MATCHKW(*lex, kw__attribute))
            {
                ParseOut__attribute__(lex, funcsp);
            }
            else if (MATCHKW(*lex, kw_alignas))
            {
                rv = true;
                *lex = getsym();
                if (needkw(lex, openpa))
                {
                    int align = 1;
                    if (startOfType(*lex, nullptr, false))
                    {
                        TYPE* tp = nullptr;
                        *lex = get_type_id(*lex, &tp, funcsp, sc_cast, false, true, false);

                        if (!tp)
                        {
                            error(ERR_TYPE_NAME_EXPECTED);
                        }
                        else if (tp->type == bt_templateparam)
                        {
                            if (tp->templateParam->second->type == kw_typename)
                            {
                                if (tp->templateParam->second->packed)
                                {
                                    std::list<TEMPLATEPARAMPAIR>* packs = tp->templateParam->second->byPack.pack;
                                    if (!MATCHKW(*lex, ellipse))
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
                                            int v = getAlign(sc_global, pack.second->byClass.val);
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
                                        align = getAlign(sc_global, tp->templateParam->second->byClass.val);
                                }
                            }
                        }
                        else
                        {
                            align = getAlign(sc_global, tp);
                        }
                    }
                    else
                    {
                        TYPE* tp = nullptr;
                        EXPRESSION* exp = nullptr;

                        *lex = optimized_expression(*lex, funcsp, nullptr, &tp, &exp, false);
                        if (!tp || !isint(tp))
                            error(ERR_NEED_INTEGER_TYPE);
                        else
                        {
                            if (!isintconst(exp))
                            {
                                align = 1;
                                if (exp->type != en_templateparam)
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
                    needkw(lex, closepa);
                    basisAttribs.inheritable.structAlign = align;
                    if (basisAttribs.inheritable.structAlign > 0x10000 ||
                        (basisAttribs.inheritable.structAlign & (basisAttribs.inheritable.structAlign - 1)) != 0)
                        error(ERR_INVALID_ALIGNMENT);
                }
            }
            else if (MATCHKW(*lex, openbr))
            {
                *lex = getsym();
                if (MATCHKW(*lex, openbr))
                {
                    const std::string occNamespace = "occ";
                    const std::string gccNamespace = "gnu";
                    const std::string clangNamespace = "clang";
                    rv = true;
                    *lex = getsym();
                    if (!MATCHKW(*lex, closebr))
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
                                using namespace std::literals;
                                std::string stripped_ver = StripUnderscores((std::string)(*lex)->data->value.s.a);
                                if (stripped_ver == occNamespace)
                                {
                                    *lex = getsym();
                                    if (MATCHKW(*lex, classsel))
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
                                    if (MATCHKW(*lex, classsel))
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
                                                        basisAttribs.inheritable.linkage2 = lk_internal;
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
                                    if (MATCHKW(*lex, classsel))
                                    {
                                        *lex = getsym();
                                        if (!ISID(*lex))
                                        {
                                            *lex = getsym();
                                            error(ERR_IDENTIFIER_EXPECTED);
                                        }
                                        else if (*lex)
                                        {
                                            // note: these are only the namespaced names listed, the __attribute__ names are
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
                                                        if (basisAttribs.inheritable.linkage2 != lk_none)
                                                            error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                                        basisAttribs.inheritable.linkage2 = lk_export;
                                                        break;
                                                    case 26:
                                                        if (basisAttribs.inheritable.linkage2 != lk_none)
                                                            error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                                        basisAttribs.inheritable.linkage2 = lk_import;
                                                        break;
                                                    case 27:
                                                        if (basisAttribs.inheritable.linkage != lk_none)
                                                            error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                                        basisAttribs.inheritable.linkage = lk_stdcall;
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

                                    if (stripped_ver == "noreturn"s)
                                    {
                                        *lex = getsym();
                                        special = true;
                                        if (basisAttribs.inheritable.linkage3 != lk_none)
                                            error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                        basisAttribs.inheritable.linkage3 = lk_noreturn;
                                    }
                                    else if (stripped_ver == "carries_dependency"s)
                                    {
                                        *lex = getsym();
                                        special = true;
                                    }
                                    else
                                    {
                                        if (stripped_ver == "deprecated"s)
                                            basisAttribs.uninheritable.deprecationText = (char*)-1;
                                        *lex = getsym();
                                        if (MATCHKW(*lex, classsel))
                                        {
                                            *lex = getsym();
                                            if (!ISID(*lex))
                                                error(ERR_IDENTIFIER_EXPECTED);
                                            *lex = getsym();
                                        }
                                    }
                                }
                            }
                            if (MATCHKW(*lex, openpa))
                            {
                                if (special)
                                    error(ERR_NO_ATTRIBUTE_ARGUMENT_CLAUSE_HERE);
                                *lex = getsym();
                                if (basisAttribs.uninheritable.deprecationText)
                                {
                                    if ((*lex)->data->type == l_astr)
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
                                    while (*lex && !MATCHKW(*lex, closepa))
                                    {
                                        balancedAttributeParameter(lex);
                                    }
                                needkw(lex, closepa);
                            }
                            if (MATCHKW(*lex, ellipse))
                                *lex = getsym();
                            if (!MATCHKW(*lex, comma))
                                break;
                            *lex = getsym();
                        }
                        if (needkw(lex, closebr))
                            needkw(lex, closebr);
                    }
                    else
                    {
                        // empty
                        *lex = getsym();
                        needkw(lex, closebr);
                    }
                }
                else
                {
                    *lex = backupsym();
                    break;
                }
            }
        }
    }
    else
    {
        ParseOut__attribute__(lex, funcsp);
    }
    return rv;
}
// these tests fall flat because they don't test the specific constructor
// used to construct things...
static bool hasNoBody(std::list<STATEMENT*>* stmts)
{
    if (stmts)
    {
        for (auto stmt : *stmts)
        {
            if (stmt->type != st_line && stmt->type != st_varstart && stmt->type != st_dbgblock)
                return false;
            // modified this next line to use 'lower'
            if (stmt->type == st_block && !hasNoBody(stmt->lower))
                return false;
        }
    }
    return true;
}
bool isConstexprConstructor(SYMBOL* sym)
{
    if (sym->sb->constexpression)
        return true;
    if (!sym->sb->deleted && !sym->sb->defaulted && !hasNoBody(sym->sb->inlineFunc.stmt))
        return false;
    for (auto sp : *sym->sb->parentClass->tp->syms)
    {
        if (ismemberdata(sp) && !sp->sb->init)
        {
            bool found = false;
            if (sym->sb->memberInitializers)
                for (auto memberInit : *sym->sb->memberInitializers)
                    if (!strcmp(memberInit->name, sp->name))
                    {
                        found = true;
                        break;
                    }
            if (!found)
                return false;
        }
    }
    return true;
}
static bool constArgValid(TYPE* tp)
{
    while (isarray(tp))
        tp = basetype(tp)->btp;
    if (isvoid(tp))
        return false;
    if (isfunction(tp))
        return false;
    if (tp->type == bt_templateparam || tp->type == bt_templateselector)
        return true;
    if (isstructured(tp))
    {
        SYMBOL *sym = basetype(tp)->sp, *sym1;
        SYMBOL *cpy, *mv;
        BASECLASS* bc;
        tp = basetype(tp);
        if (sym->sb->trivialCons)
            return true;
        tp = PerformDeferredInitialization(tp, nullptr);
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
                if (sp->sb->storage_class == sc_member && !isfunction(sp->tp))
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
    if (sym->sb->storage_class == sc_virtual)
        return false;
    if (!constArgValid(basetype(sym->tp)->btp))
        return false;
    for (auto sym1 : *basetype(sym->tp)->syms)
    {
        if (sym1->tp->type != bt_void && !constArgValid(sym1->tp))
            return false;
    }
    return true;
}
LEXLIST* getDeclType(LEXLIST* lex, SYMBOL* funcsp, TYPE** tn)
{
    bool hasAmpersand = false;
    bool hasAuto = false;
    EXPRESSION *exp, *exp2;
    lex = getsym();
    needkw(&lex, openpa);
    bool extended = MATCHKW(lex, openpa);
    hasAmpersand = MATCHKW(lex, andx);
    if (extended || hasAmpersand)
    {
        lex = getsym();
        hasAuto = MATCHKW(lex, kw_auto);
        lex = backupsym();
    }
    else
    {
        hasAuto = MATCHKW(lex, kw_auto);
    }
    if (hasAuto)
    {
        if (extended || hasAmpersand)
            lex = getsym();
        lex = getsym();
        if (MATCHKW(lex, andx) || MATCHKW(lex, land))
        {
            lex = getsym();
            error(ERR_DECLTYPE_AUTO_NO_REFERENCE);
        }
        if (extended)
            needkw(&lex, closepa);
        (*tn) = MakeType(bt_auto);
        (*tn)->size = 0;
        (*tn)->decltypeauto = true;
        (*tn)->decltypeautoextended = extended;
    }
    else
    {
        auto oldnoExcept = noExcept;
        lex = expression_no_check(lex, nullptr, nullptr, &(*tn), &exp, _F_SIZEOF | _F_INDECLTYPE);
        noExcept = oldnoExcept;
        if (exp->type == en_func && exp->v.func->sp->sb->deleted)
        {
            *tn = &stdany;
            return lex;
        }
        if ((*tn) && (*tn)->type == bt_aggregate && exp->type == en_func)
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
                    (*tn) = MakeType(bt_memberptr, exp->v.func->sp->tp);
                    (*tn)->sp = exp->v.func->sp->sb->parentClass;
                }
                else
                {
                    (*tn) = MakeType(bt_pointer, exp->v.func->sp->tp);
                    (*tn)->size = getSize(bt_pointer);
                }
                exp->v.func->functp = (*tn)->btp;
            }
            else
            {
                (*tn) = exp->v.func->functp = exp->v.func->sp->tp;
            }
        }
        if ((*tn) && isfunction(*tn) && exp->type == en_func)
        {
            if (exp->v.func->ascall)
            {
                *tn = basetype(*tn)->btp;
            }
        }
        if ((*tn))
        {
            optimize_for_constants(&exp);
            if (templateNestingCount && !instantiatingTemplate)
            {
                (*tn) = MakeType(bt_templatedecltype);
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
            (*tn) = MakeType(bt_lref, *tn);
        }
        else if ((*tn)->rref)
        {
            (*tn) = MakeType(bt_rref, *tn);
        }
        if (extended && lvalue(exp) && exp->left->type == en_auto)
        {
            if (!lambdas.size() && xvalue(exp))
            {
                if (isref((*tn)))
                    (*tn) = basetype((*tn))->btp;
                (*tn) = MakeType(bt_rref, *tn);
            }
            else if (lvalue(exp))
            {
                if (isref((*tn)))
                    (*tn) = basetype((*tn))->btp;
                (*tn) = MakeType(bt_lref, *tn);
                if (lambdas.size() && !lambdas.front()->isMutable)
                {
                    (*tn)->btp = MakeType(bt_const, (*tn)->btp);
                }
            }
        }
    }
    needkw(&lex, closepa);
    return lex;
}

void CollapseReferences(TYPE* tp_in)
{
    TYPE* tp1 = tp_in;
    int count = 0;
    bool lref = false;
    while (tp1 && isref(tp1))
    {
        if (basetype(tp1)->type == bt_lref)
            lref = true;
        count++;
        tp1 = basetype(tp1)->btp;
    }
    if (count > 1)
    {
        tp1 = tp_in;
        basetype(tp1)->type = lref ? bt_lref : bt_rref;
        tp1 = basetype(tp1);
        while (basetype(tp1->btp) && isref(tp1->btp))
        {
            // yes we intend to get rid of top-level CV qualifiers, reference collapsing demands it.
            tp1->btp = tp1->btp->btp;
        }
        UpdateRootTypes(tp_in);
    }
    else if (count == 1 && isvoid(basetype(tp_in)->btp))
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
            while (*last && (*last)->type == en_void)
                last = &(*last)->right;
            if (*last)
            {
                *last = exprNode(en_void, *last, nullptr);
                last = &(*last)->right;
            }
            auto newFunc = makeID(sc_global, &stdfunc, nullptr, litlate((std::string(decl->sb->decoratedName) + "_dest").c_str()));
            SetLinkerNames(newFunc, lk_c);
            optimize_for_constants(&decl->sb->dest->front()->exp);
            auto body = decl->sb->dest->front()->exp;
            InsertLocalStaticUnInitializer(newFunc, body);

            EXPRESSION* callexp = exprNode(en_func, nullptr, nullptr);
            callexp->v.func = Allocate<FUNCTIONCALL>();
            callexp->v.func->sp = atexitfunc;
            callexp->v.func->functp = atexitfunc->tp;
            callexp->v.func->fcall = varNode(en_pc, atexitfunc);
            callexp->v.func->ascall = true;
            callexp->v.func->arguments = initListListFactory.CreateList();
            auto arg = Allocate<INITLIST>();
            callexp->v.func->arguments->push_back(arg);
            arg->tp = &stdpointer;
            arg->exp = varNode(en_pc, newFunc);
            *last = callexp;
        }
    }
    return exp;
}
void CheckIsLiteralClass(TYPE* tp)
{
    if (!templateNestingCount || instantiatingTemplate)
    {
        if (isref(tp))
            tp = basetype(tp)->btp;
        if (isstructured(tp))
        {
            if (basetype(tp)->sp->tp->size && !basetype(tp)->sp->sb->literalClass)
            {
                errorsym(ERR_CONSTEXPR_CLASS_NOT_LITERAL, basetype(tp)->sp);
            }
            else if (basetype(tp)->sp->templateParams)
            {
                for (auto&& tpl : *basetype(tp)->sp->templateParams)
                    if (tpl.second->type == kw_typename && tpl.second->byClass.val)
                        CheckIsLiteralClass(tpl.second->byClass.val);               
            }
        }
    }
}
}  // namespace Parser