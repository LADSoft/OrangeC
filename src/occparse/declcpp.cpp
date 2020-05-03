/* Software License Agreement
 * 
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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
#include "symtab.h"
#include "mangle.h"
#include "initbackend.h"
#include "occparse.h"
#include "template.h"
#include "declare.h"
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

#ifdef PARSER_ONLY
namespace CompletionCompiler
{
    void ccInsertUsing(Parser::SYMBOL* ns, Parser::SYMBOL* parentns, const char* file, int line);
}
#endif
namespace Parser
{

    attributes basisAttribs;

    Optimizer::LIST* nameSpaceList;
    char anonymousNameSpaceName[512];

    static Optimizer::LIST* deferred;


    static int dumpVTabEntries(int count, THUNK* thunks, SYMBOL* sym, VTABENTRY* entry)
    {
    #ifndef PARSER_ONLY
        while (entry)
        {
            if (!entry->isdead)
            {
                VIRTUALFUNC* vf = entry->virtuals;
                Optimizer::genaddress(entry->dataOffset);
                Optimizer::genaddress(entry->vtabOffset);
                while (vf)
                {
                    if (vf->func->sb->deferredCompile && (!vf->func->sb->templateLevel || vf->func->sb->instantiated))
                    {
                        FUNCTIONCALL fcall;
                        TYPE* tp = nullptr;
                        EXPRESSION* exp = intNode(en_c_i, 0);
                        SYMBOL* sp = vf->func->sb->overloadName;
                        INITLIST** args = &fcall.arguments;
                        SYMLIST* hr = basetype(vf->func->tp)->syms->table[0];
                        memset(&fcall, 0, sizeof(fcall));
                        while (hr)
                        {
                            SYMBOL* sym = hr->p;
                            if (sym->sb->thisPtr)
                            {
                                fcall.thistp = sym->tp;
                                fcall.thisptr = exp;
                            }
                            else if (sym->tp->type != bt_void)
                            {
                                *args = (INITLIST*)Alloc(sizeof(INITLIST));
                                (*args)->tp = sym->tp;
                                (*args)->exp = exp;
                                args = &(*args)->next;
                            }
                            hr = hr->next;
                        }
                        fcall.ascall = true;
                        sp = GetOverloadedFunction(&tp, &exp, sp, &fcall, nullptr, true, false, true, 0);
                        if (sp)
                            vf->func = sp;
                    }
                    InsertInline(vf->func);
                    if (vf->func->sb->ispure)
                    {
                        Optimizer::genaddress(0);
                    }
                    else if (sym == vf->func->sb->parentClass && entry->vtabOffset)
                    {
                        char buf[512];
                        SYMBOL* localsp;
                        strcpy(buf, sym->sb->decoratedName);
                        Optimizer::my_sprintf(buf + strlen(buf), "_$%c%d", count % 26 + 'A', count / 26);

                        thunks[count].entry = entry;
                        if (vf->func->sb->attribs.inheritable.linkage2 == lk_import)
                        {
                            EXPRESSION* exp = varNode(en_pc, vf->func);
                            thunkForImportTable(&exp);
                            thunks[count].func = exp->v.sp;
                        }
                        else
                        {
                            thunks[count].func = vf->func;
                        }
                        thunks[count].name = localsp = makeID(sc_static, &stdfunc, nullptr, litlate(buf));
                        localsp->sb->decoratedName = localsp->name;
                        localsp->sb->attribs.inheritable.linkage = lk_virtual;
                        genref(Optimizer::SymbolManager::Get(localsp), 0);
                        InsertInline(localsp);
                        count++;
                    }
                    else
                    {
                        if (vf->func->sb->attribs.inheritable.linkage2 == lk_import)
                        {
                            EXPRESSION* exp = varNode(en_pc, vf->func);
                            thunkForImportTable(&exp);
                            genref(Optimizer::SymbolManager::Get(exp->v.sp), 0);
                        }
                        else
                        {
                            genref(Optimizer::SymbolManager::Get(vf->func), 0);
                        }
                    }
                    vf = vf->next;
                }
            }
            count = dumpVTabEntries(count, thunks, sym, entry->children);
            entry = entry->next;
        }
    #else
        count = 0;
    #endif
        return count;
    }
    void dumpVTab(SYMBOL* sym)
    {
    #ifndef PARSER_ONLY
        THUNK thunks[1000];
        SYMBOL* xtSym = RTTIDumpType(basetype(sym->tp));
        int count = 0;

        Optimizer::dseg();
        gen_virtual(Optimizer::SymbolManager::Get(sym->sb->vtabsp), true);
        if (xtSym)
            genref(Optimizer::SymbolManager::Get(xtSym), 0);
        else
            Optimizer::genaddress(0);
        count = dumpVTabEntries(count, thunks, sym, sym->sb->vtabEntries);
        gen_endvirtual(Optimizer::SymbolManager::Get(sym->sb->vtabsp));

        if (count)
        {
            int i;
            Optimizer::cseg();
            for (i = 0; i < count; i++)
            {
                gen_virtual(Optimizer::SymbolManager::Get(thunks[i].name), false);
                gen_vtt(-(int)thunks[i].entry->dataOffset, Optimizer::SymbolManager::Get(thunks[i].func), Optimizer::SymbolManager::Get(thunks[i].name));
                gen_endvirtual(Optimizer::SymbolManager::Get(thunks[i].name));
            }
        }
    #endif
    }
    void internalClassRefCount(SYMBOL* base, SYMBOL* derived, int* vcount, int* ccount, bool isVirtual)
    {
        if (base == derived || (base && derived && sameTemplate(derived->tp, base->tp)))
            (*ccount)++;
        else
        {
            if (base && derived && derived->sb)
            {
                BASECLASS* lst = derived->sb->baseClasses;
                while (lst)
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
                    lst = lst->next;
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
        if (rv)
        {
            TYPE *tp1 = basetype(oldFunc->tp)->btp, *tp2 = basetype(newFunc->tp)->btp;
            if (!comparetypes(tp1, tp2, true) && !sameTemplate(tp1, tp2) &&
                (!isstructured(tp1) || !isstructured(tp2) ||
                 (basetype(tp1)->sp->sb->templateLevel && basetype(tp2)->sp->sb->parentTemplate != basetype(tp1)->sp->sb->parentTemplate)))
            {
                if (!templateNestingCount)
                {
                    if (basetype(tp1)->type == basetype(tp2)->type)
                    {
                        if (ispointer(tp1) || isref(tp1))
                        {
                            if (isconst(tp1) == isconst(tp2) && isvolatile(tp1) == isvolatile(tp2))
                            {
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
    static bool backpatchVirtualFunc(SYMBOL* sym, VTABENTRY* entry, SYMBOL* func)
    {
        bool rv = false;
        while (entry)
        {
            VIRTUALFUNC* vf = entry->virtuals;
            while (vf)
            {
                if (vfMatch(sym, vf->func, func))
                {
                    if (vf->func->sb->isfinal)
                        errorsym(ERR_OVERRIDING_FINAL_VIRTUAL_FUNC, vf->func);
                    if (vf->func->sb->deleted != func->sb->deleted)
                        errorsym2(ERR_DELETED_VIRTUAL_FUNC, vf->func->sb->deleted ? vf->func : func, vf->func->sb->deleted ? func : vf->func);

                    vf->func = func;
                    rv = true;
                }
                vf = vf->next;
            }
            backpatchVirtualFunc(sym, entry->children, func);
            entry = entry->next;
        }
        return rv;
    }
    static int allocVTabSpace(VTABENTRY* vtab, int offset)
    {
        while (vtab)
        {
            VIRTUALFUNC* vf;
            if (!vtab->isdead)
            {
                vtab->vtabOffset = offset;
                vf = vtab->virtuals;
                while (vf)
                {
                    offset += getSize(bt_pointer);
                    vf = vf->next;
                }
                offset += 2 * getSize(bt_pointer);
            }
            offset = allocVTabSpace(vtab->children, offset);
            vtab = vtab->next;
        }
        return offset;
    }
    static void copyVTabEntries(VTABENTRY* lst, VTABENTRY** pos, int offset, bool isvirtual)
    {
        while (lst)
        {
            VTABENTRY* vt = (VTABENTRY*)Alloc(sizeof(VTABENTRY));
            VIRTUALFUNC *vf, **vfc;
            vt->cls = lst->cls;
            vt->isvirtual = lst->isvirtual;
            vt->isdead = vt->isvirtual || lst->dataOffset == 0 || isvirtual;
            vt->dataOffset = offset + lst->dataOffset;
            *pos = vt;
            vf = lst->virtuals;
            vfc = &vt->virtuals;
            while (vf)
            {
                *vfc = (VIRTUALFUNC*)Alloc(sizeof(VIRTUALFUNC));
                (*vfc)->func = vf->func;
                vfc = &(*vfc)->next;
                vf = vf->next;
            }
            if (lst->children)
                copyVTabEntries(lst->children, &vt->children, offset + lst->dataOffset,
                                vt->isvirtual || isvirtual || lst->dataOffset == lst->children->dataOffset);
            pos = &(*pos)->next;
            lst = lst->next;
        }
    }
    static void checkAmbiguousVirtualFunc(SYMBOL* sym, VTABENTRY** match, VTABENTRY* vt)
    {
        while (vt)
        {
            if (sym == vt->cls)
            {
                if (!*match)
                {
                    (*match) = vt;
                }
                else
                {
                    VIRTUALFUNC* f1 = (*match)->virtuals;
                    VIRTUALFUNC* f2 = vt->virtuals;
                    while (f1 && f2)
                    {
                        if (f1->func != f2->func)
                        {
                            errorsym2(ERR_AMBIGUOUS_VIRTUAL_FUNCTIONS, f1->func, f2->func);
                        }
                        f1 = f1->next;
                        f2 = f2->next;
                    }
                }
            }
            else
            {
                checkAmbiguousVirtualFunc(sym, match, vt->children);
            }
            vt = vt->next;
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
                if ((sym1->sb->xcMode == xc_dynamic && sym1->sb->xc->xcDynamic) || sym1->sb->xcMode == xc_unspecified || sym1->sb->xcMode == xc_all)
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
        SYMLIST* hr = basetype(sp->tp)->syms->table[0];
        while (hr)
        {
            SYMBOL* sym = hr->p;
            if (sym->sb->storage_class == sc_overloads)
            {
                SYMLIST* hr1 = basetype(sym->tp)->syms->table[0];
                while (hr1)
                {
                    SYMBOL* sym1 = (SYMBOL*)hr1->p;
                    if (sym1->sb->storage_class == sc_virtual)
                    {
                        BASECLASS* bc = sp->sb->baseClasses;
                        const char* f1 = strrchr(sym1->sb->decoratedName, '@');
                        while (bc && f1)
                        {
                            SYMBOL* sym2 = search(sym->name, basetype(bc->cls->tp)->syms);
                            if (sym2)
                            {
                                SYMLIST* hr3 = basetype(sym2->tp)->syms->table[0];
                                while (hr3)
                                {
                                    SYMBOL* sym3 = (SYMBOL*)hr3->p;
                                    const char* f2 = strrchr(sym3->sb->decoratedName, '@');
                                    if (f2 && !strcmp(f1, f2))
                                    {
                                        checkXT(sym1, sym3, false);
                                    }
                                    hr3 = hr3->next;
                                }
                            }
                            bc = bc->next;
                        }
                    }
                    hr1 = hr1->next;
                }
            }
            hr = hr->next;
        }
    }
    void CheckCalledException(SYMBOL* cst, EXPRESSION* exp)
    {
        (void)exp;
    //    if (cst->sb->xcMode != xc_none && (cst->sb->xcMode != xc_dynamic || (cst->sb->xc && cst->sb->xc->xcDynamic)))
    //        functionCanThrow = true;
    }
    void calculateVTabEntries(SYMBOL* sym, SYMBOL* base, VTABENTRY** pos, int offset)
    {
        BASECLASS* lst = base->sb->baseClasses;
        VBASEENTRY* vb = sym->sb->vbaseEntries;
        SYMLIST* hr = sym->tp->syms->table[0];
        if (sym->sb->hasvtab && (!lst || lst->isvirtual || !lst->cls->sb->vtabEntries))
        {
            VTABENTRY* vt = (VTABENTRY*)Alloc(sizeof(VTABENTRY));
            vt->cls = sym;
            vt->isvirtual = false;
            vt->isdead = false;
            vt->dataOffset = offset;
            *pos = vt;
            pos = &(*pos)->next;
        }
        while (lst)
        {
            VTABENTRY* vt = (VTABENTRY*)Alloc(sizeof(VTABENTRY));
            vt->cls = lst->cls;
            vt->isvirtual = lst->isvirtual;
            vt->isdead = vt->isvirtual;
            vt->dataOffset = offset + lst->offset;
            *pos = vt;
            pos = &(*pos)->next;
            copyVTabEntries(lst->cls->sb->vtabEntries, &vt->children, lst->offset, false);
            if (vt->children)
            {
                VIRTUALFUNC *vf, **vfc;
                vf = vt->children->virtuals;
                vfc = &vt->virtuals;
                while (vf)
                {
                    *vfc = (VIRTUALFUNC*)Alloc(sizeof(VIRTUALFUNC));
                    (*vfc)->func = vf->func;
                    vfc = &(*vfc)->next;
                    vf = vf->next;
                }
            }
            lst = lst->next;
        }
        while (hr && sym->sb->vtabEntries)
        {
            SYMBOL* cur = hr->p;
            if (cur->sb->storage_class == sc_overloads)
            {
                SYMLIST* hrf = cur->tp->syms->table[0];
                while (hrf)
                {
                    SYMBOL* cur = (SYMBOL*)hrf->p;
                    VTABENTRY *hold, *hold2;
                    bool found = false;
                    bool isfirst = false;
                    bool isvirt = cur->sb->storage_class == sc_virtual;
                    hold = sym->sb->vtabEntries->next;
                    hold2 = sym->sb->vtabEntries->children;
                    sym->sb->vtabEntries->next = nullptr;
                    sym->sb->vtabEntries->children = nullptr;
                    found = backpatchVirtualFunc(sym, hold, cur);
                    found |= backpatchVirtualFunc(sym, hold2, cur);
                    isfirst = backpatchVirtualFunc(sym, sym->sb->vtabEntries, cur);
                    isvirt |= found | isfirst;
                    sym->sb->vtabEntries->next = hold;
                    sym->sb->vtabEntries->children = hold2;
                    if (isvirt)
                    {
                        cur->sb->storage_class = sc_virtual;
                        if (!isfirst)
                        {
                            VIRTUALFUNC** vf;
                            vf = &sym->sb->vtabEntries->virtuals;
                            while (*vf)
                                vf = &(*vf)->next;
                            *vf = (VIRTUALFUNC*)Alloc(sizeof(VIRTUALFUNC));
                            (*vf)->func = cur;
                        }
                    }
                    if (cur->sb->isoverride && !found && !isfirst)
                    {
                        errorsym(ERR_FUNCTION_DOES_NOT_OVERRIDE, cur);
                    }
                    hrf = hrf->next;
                }
            }
            hr = hr->next;
        }
        vb = sym->sb->vbaseEntries;
        while (vb)
        {
            if (vb->alloc)
            {
                VTABENTRY* match = nullptr;
                checkAmbiguousVirtualFunc(vb->cls, &match, sym->sb->vtabEntries);
            }
            vb = vb->next;
        }
        checkExceptionSpecification(sym);
        allocVTabSpace(sym->sb->vtabEntries, 0);
        if (sym->sb->vtabEntries)
        {
            VIRTUALFUNC* vf = sym->sb->vtabEntries->virtuals;
            int ofs = 0;
            while (vf)
            {
                if (vf->func->sb->parentClass == sym)
                {
                    vf->func->sb->vtaboffset = ofs;
                }
                ofs += getSize(bt_pointer);
                vf = vf->next;
            }
        }
    }
    void calculateVirtualBaseOffsets(SYMBOL* sym)
    {
        BASECLASS* lst = sym->sb->baseClasses;
        VBASEENTRY **pos = &sym->sb->vbaseEntries, *vbase;
        // copy all virtual base classes of direct base classes
        while (lst)
        {
            VBASEENTRY* cur = lst->cls->sb->vbaseEntries;
            while (cur)
            {
                VBASEENTRY* search;
                vbase = (VBASEENTRY*)Alloc(sizeof(VBASEENTRY));
                vbase->alloc = false;
                vbase->cls = cur->cls;
                vbase->pointerOffset = cur->pointerOffset + lst->offset;
                vbase->structOffset = 0;
                *pos = vbase;
                pos = &(*pos)->next;

                search = sym->sb->vbaseEntries;
                while (search)
                {
                    if (search->cls == vbase->cls && search->alloc)
                        break;
                    search = search->next;
                }
                if (!search)
                {
                    // copy for the derived class's vbase table
                    vbase = (VBASEENTRY*)Alloc(sizeof(VBASEENTRY));
                    vbase->alloc = true;
                    vbase->cls = cur->cls;
                    vbase->pointerOffset = 0;
                    vbase->structOffset = 0;
                    *pos = vbase;
                    pos = &(*pos)->next;
                }
                cur = cur->next;
            }
            lst = lst->next;
        }
        // now add any new base classes for this derived class
        lst = sym->sb->baseClasses;
        while (lst)
        {
            if (lst->isvirtual)
            {
                VBASEENTRY* search = sym->sb->vbaseEntries;
                while (search)
                {
                    if (search->cls == lst->cls && search->alloc)
                        break;
                    search = search->next;
                }
                if (!search)
                {
                    vbase = (VBASEENTRY*)Alloc(sizeof(VBASEENTRY));
                    vbase->alloc = true;
                    vbase->cls = lst->cls;
                    vbase->pointerOffset = 0;
                    vbase->structOffset = 0;
                    *pos = vbase;
                    pos = &(*pos)->next;
                }
            }
            lst = lst->next;
        }
        // modify virtual base thunks for self
        vbase = sym->sb->vbaseEntries;
        while (vbase)
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
                base = sym->sb->baseClasses;
                while (base)
                {
                    if (base->isvirtual && base->cls == vbase->cls)
                    {
                        if (base != sym->sb->baseClasses)
                            base->offset = sym->tp->size;
                        break;
                    }
                    base = base->next;
                }
                vbase->pointerOffset = sym->tp->size;
                sym->tp->size += getSize(bt_pointer);
            }
            vbase = vbase->next;
        }
        sym->sb->sizeNoVirtual = sym->tp->size;
        vbase = sym->sb->vbaseEntries;
        // now add space for virtual base classes
        while (vbase)
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
                cur = sym->sb->vbaseEntries;
                while (cur)
                {
                    if (cur->cls == vbase->cls)
                    {
                        cur->structOffset = sym->tp->size;
                    }
                    cur = cur->next;
                }
                if (vbase->cls->sb->maintemplate && !vbase->cls->sb->sizeNoVirtual)
                    n = vbase->cls->sb->maintemplate->sb->sizeNoVirtual;
                else
                    n = vbase->cls->sb->sizeNoVirtual;
                sym->tp->size += n;
            }
            vbase = vbase->next;
        }
    }
    void deferredCompileOne(SYMBOL* cur)
    {
        LEXEME* lex;
        STRUCTSYM l, n, x, q;
        int count = 0;
        LAMBDA* oldLambdas;
        // function body
        if (!cur->sb->inlineFunc.stmt && (!cur->sb->templateLevel || !cur->templateParams || cur->sb->instantiated))
        {
            int tns = PushTemplateNamespace(cur->sb->parentClass);
            Optimizer::LINEDATA *linesHeadOld = linesHead, *linesTailOld = linesTail;
            linesHead = linesTail = nullptr;

            cur->sb->attribs.inheritable.linkage = lk_virtual;
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
            lambdas = nullptr;
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
            linesHead = linesHeadOld;
            linesTail = linesTailOld;
        }
    }
    static void RecalcArraySize(TYPE* tp)
    {
        if (isarray(basetype(tp)->btp))
            RecalcArraySize(basetype(tp)->btp);
        tp->size = basetype(basetype(tp)->btp)->size * basetype(tp)->esize->v.i;
    }
    void deferredInitializeStructFunctions(SYMBOL* cur)
    {
        SYMLIST* hr;
        LEXEME* lex;
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
        hr = cur->tp->syms->table[0];
        while (hr)
        {
            SYMBOL* sp = hr->p;
            if (sp->sb->storage_class == sc_overloads)
            {
                if (templateNestingCount != 1 || instantiatingTemplate)
                {
                    SYMLIST* hr1 = sp->tp->syms->table[0];
                    while (hr1)
                    {
                        SYMBOL* sp1 = (SYMBOL*)hr1->p;
                        if (!sp1->sb->templateLevel)
                        {
                            SYMLIST* hr2 = basetype(sp1->tp)->syms->table[0];
                            while (hr2)
                            {
                                SYMBOL* sp2 = (SYMBOL*)hr2->p;
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
                                        if (isstructured(tp2))
                                        {
                                            SYMBOL* sym;
                                            anonymousNotAlloc++;
                                            sym = anonymousVar(sc_auto, tp2)->v.sp;
                                            anonymousNotAlloc--;
                                            sym->sb->stackblock = !isref(sp2->tp);
                                            lex = initialize(lex, theCurrentFunc, sym, sc_auto, false, 0); /* also reserves space */
                                            sp2->sb->init = sym->sb->init;
                                            sym->sb->allocate = false;
                                            if (sp2->sb->init->exp->type == en_thisref)
                                            {
                                                EXPRESSION** expr = &sp2->sb->init->exp->left->v.func->thisptr;
                                                if ((*expr)->type == en_add && isconstzero(&stdint, (*expr)->right))
                                                    sp2->sb->init->exp->v.t.thisptr = (*expr) = (*expr)->left;
                                            }
                                        }
                                        else
                                        {
                                            lex = initialize(lex, theCurrentFunc, sp2, sc_member, false, 0);
                                        }
                                        SetAlternateLex(nullptr);
                                        sp2->sb->deferredCompile = nullptr;
                                    }
                                }
                                hr2 = hr2->next;
                            }
                        }
                        hr1 = hr1->next;
                    }
                }
            }
            hr = hr->next;
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
        SYMLIST* hr;
        LEXEME* lex;
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
        hr = cur->tp->syms->table[0];
        while (hr)
        {
            SYMBOL* sp = hr->p;
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
                lex = initialize(lex, theCurrentFunc, sp, sc_member, false, 0);
                SetAlternateLex(nullptr);
            }
            hr = hr->next;
        }
        staticAssert = cur->sb->staticAsserts;
        while (staticAssert)
        {
            lex = SetAlternateLex((LEXEME*)staticAssert->data);
            lex = handleStaticAssert(lex);
            SetAlternateLex(nullptr);
            staticAssert = staticAssert->next;
        }
        dontRegisterTemplate--;
        while (count--)
        {
            dropStructureDeclaration();
        }
        PopTemplateNamespace(tns);
    }
    static bool declaringTemplate(SYMBOL* sym)
    {
        STRUCTSYM* l = structSyms;
        while (l)
        {
            if (l->str && l->str->sb->templateLevel)
            {
                if (sym->sb->decoratedName && !strcmp(sym->sb->decoratedName, l->str->sb->decoratedName))
                    return true;
            }
            l = l->next;
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
        TYPE **tpx1 = tpx;
        while ((*tpx)->btp && !isfunction(*tpx))
            tpx = &(*tpx)->btp;
        if (Optimizer::cparams.prm_cplusplus && !inTemplateType && isstructured(*tpx))
        {
            SYMBOL* sym = basetype(*tpx)->sp;
            TEMPLATEPARAMLIST* tpl = sym->templateParams;
            while (tpl)
            {
                if (tpl->p->usedAsUnpacked)
                    return tp;
                tpl = tpl->next;
            }
            if (declaringTemplate(sym))
            {
                if (!sym->sb->instantiated)
                {
                    *tpx = sym->tp;
                }
            }
            else if (sym->sb->templateLevel && (!sym->sb->instantiated || sym->sb->attribs.inheritable.linkage != lk_virtual) && sym->templateParams &&
                     allTemplateArgsSpecified(sym, sym->templateParams->next))
            {
                sym = TemplateClassInstantiateInternal(sym, nullptr, false);
                if (sym)
                    *tpx = sym->tp;
            }
            else if (!sym->sb->templateLevel && sym->sb->parentClass && sym->sb->parentClass->sb->templateLevel &&
                     (!sym->sb->instantiated || sym->sb->attribs.inheritable.linkage != lk_virtual) && sym->sb->parentClass->templateParams &&
                     allTemplateArgsSpecified(sym->sb->parentClass, sym->sb->parentClass->templateParams->next))
            {
                TEMPLATEPARAMLIST* tpl = sym->sb->parentClass->templateParams;
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
        SYMLIST* hr = sym->tp->syms->table[0];
        hr = sym->tp->syms->table[0];
        while (hr)
        {
            SYMBOL* cur = hr->p;
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
                SYMLIST* hrf = cur->tp->syms->table[0];
                while (hrf)
                {
                    cur = (SYMBOL*)hrf->p;
                    if (localClassWarnings)
                    {
                        if (isfunction(cur->tp))
                            if (!basetype(cur->tp)->sp->sb->inlineFunc.stmt)
                                errorsym(ERR_LOCAL_CLASS_FUNCTION_NEEDS_BODY, cur);
                    }
                    if (cur->sb->isfinal || cur->sb->isoverride || cur->sb->ispure)
                        if (cur->sb->storage_class != sc_virtual)
                            error(ERR_SPECIFIER_VIRTUAL_FUNC);
                    hrf = hrf->next;
                }
            }
            hr = hr->next;
        }
    }
    bool usesVTab(SYMBOL* sym)
    {
        SYMLIST* hr;
        BASECLASS* base;
        hr = sym->tp->syms->table[0];
        while (hr)
        {
            SYMBOL* cur = hr->p;
            if (cur->sb->storage_class == sc_overloads)
            {
                SYMLIST* hrf = cur->tp->syms->table[0];
                while (hrf)
                {
                    if (((SYMBOL*)(hrf->p))->sb->storage_class == sc_virtual)
                        return true;
                    hrf = hrf->next;
                }
            }
            hr = hr->next;
        }
        base = sym->sb->baseClasses;
        while (base)
        {
            if (base->cls->sb->hasvtab)
                return true;
            base = base->next;
        }
        return false;
    }
    BASECLASS* innerBaseClass(SYMBOL* declsym, SYMBOL* bcsym, bool isvirtual, enum e_ac currentAccess)
    {
        BASECLASS* bc;
        BASECLASS* t = declsym->sb->baseClasses;
        while (t)
        {
            if (t->cls == bcsym)
            {
                errorsym(ERR_BASE_CLASS_INCLUDED_MORE_THAN_ONCE, bcsym);
                break;
            }
            t = t->next;
        }
        if (basetype(bcsym->tp)->type == bt_union)
            error(ERR_UNION_CANNOT_BE_BASE_CLASS);
        if (bcsym->sb->isfinal)
            errorsym(ERR_FINAL_BASE_CLASS, bcsym);
        bc = (BASECLASS*)Alloc(sizeof(BASECLASS));
        bc->accessLevel = currentAccess;
        bc->isvirtual = isvirtual;
        bc->cls = bcsym;
        // inject it in the class so we can look it up as a type later
        //
        if (!t)
        {
            SYMBOL* injected = clonesym(bcsym);
            injected->name = injected->sb->decoratedName;  // for nested templates
            injected->sb->mainsym = bcsym;
            injected->sb->parentClass = declsym;
            injected->sb->access = currentAccess;
            insert(injected, declsym->tp->tags);
        }
        return bc;
    }
    LEXEME* baseClasses(LEXEME* lex, SYMBOL* funcsp, SYMBOL* declsym, enum e_ac defaultAccess)
    {
        struct _baseClass **bc = &declsym->sb->baseClasses, *lst;
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
                lex = get_type_id(lex, &tp, funcsp, sc_type, true, true);
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
                    *bc = innerBaseClass(declsym, tp->sp, isvirtual, currentAccess);
                    if (*bc)
                        bc = &(*bc)->next;
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
                    strcpy(name, lex->value.s.a);
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
                        TEMPLATEPARAMLIST* nullLst;
                        lex = GetTemplateArguments(lex, funcsp, bcsym, &nullLst);
                        inTemplateSpecialization--;
                    }
                    bcsym = nullptr;
                }
                else if (bcsym && bcsym->sb && bcsym->sb->templateLevel)
                {
                    if (bcsym->sb->storage_class == sc_typedef)
                    {
                        if (MATCHKW(lex, lt))
                        {
                            // throwaway
                            TEMPLATEPARAMLIST* lst = nullptr;
                            SYMBOL* sp1;
                            inTemplateSpecialization++;
                            lex = GetTemplateArguments(lex, funcsp, bcsym, &lst);
                            inTemplateSpecialization--;
                            sp1 = GetTypedefSpecialization(bcsym, lst);
                            if (sp1)
                            {
                                bcsym = sp1;
                                if (isstructured(bcsym->tp))
                                    bcsym->tp = PerformDeferredInitialization(bcsym->tp, funcsp);
                                else
                                    bcsym->tp = SynthesizeType(bcsym->tp, nullptr, false);
                                //                            if (isstructured(bcsym->tp))
                                //                                bcsym = basetype(bcsym->tp)->sp;
                            }
                        }
                        else
                        {
                            if (!noSpecializationError && !instantiatingTemplate)
                                errorsym(ERR_NEED_SPECIALIZATION_PARAMETERS, bcsym);
                        }
                    }
                    else
                    {
                        TEMPLATEPARAMLIST* lst = nullptr;
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
                                        *bc = innerBaseClass(declsym, bcsym, isvirtual, currentAccess);
                                        if (*bc)
                                            bc = &(*bc)->next;
                                    }
                                }
                                else
                                {
                                    int n = 0;
                                    bool done = false;
                                    bool failed = false;
                                    TEMPLATEPARAMLIST* dest;
                                    TEMPLATEPARAMLIST* src;
                                    while (!done)
                                    {
                                        TEMPLATEPARAMLIST *workingList = nullptr, **workingListPtr = &workingList;
                                        SYMBOL* temp;
                                        bool packed = false;
                                        dest = bcsym->templateParams->next;
                                        src = lst;
                                        while (src && dest)
                                        {
                                            *workingListPtr = (TEMPLATEPARAMLIST*)(TEMPLATEPARAMLIST*)Alloc(sizeof(TEMPLATEPARAMLIST));
                                            (*workingListPtr)->p = (TEMPLATEPARAM*)(TEMPLATEPARAM*)Alloc(sizeof(TEMPLATEPARAM));
                                            if (src->p->packed)
                                            {
                                                TEMPLATEPARAMLIST* p = src->p->byPack.pack;
                                                packed = true;
                                                for (i = 0; i < n && p; i++)
                                                {
                                                    p = p->next;
                                                }
                                                if (!p)
                                                {
                                                    done = true;
                                                    break;
                                                }
                                                *(*workingListPtr)->p = *(p->p);
                                                (*workingListPtr)->p->byClass.dflt = p->p->byClass.val;
                                            }
                                            else
                                            {
                                                *(*workingListPtr)->p = *(src->p);
                                                (*workingListPtr)->p->byClass.dflt = src->p->byClass.val;
                                            }
                                            workingListPtr = &(*workingListPtr)->next;
                                            dest = dest->next;
                                            src = src->next;
                                        }
                                        if (done)
                                            break;
                                        if (workingList)
                                        {
                                            temp = GetClassTemplate(bcsym, workingList, true);
                                            if (temp && allTemplateArgsSpecified(temp, temp->templateParams->next))
                                                temp = TemplateClassInstantiateInternal(temp, temp->templateParams->next, false);
                                            if (temp)
                                            {
                                                *bc = innerBaseClass(declsym, temp, isvirtual, currentAccess);
                                                if (*bc)
                                                {
                                                    bc = &(*bc)->next;
                                                }
                                            }
                                            n++;
                                        }
                                        if (!packed)
                                            break;
                                    }
                                    dest = bcsym->templateParams->next;
                                    src = lst;
                                    while (src && dest)
                                    {
                                        if (src->p->packed && src->p->byPack.pack)
                                        {
                                            TEMPLATEPARAMLIST* p = src->p->byPack.pack;
                                            for (i = 0; i < n && p; i++)
                                            {
                                                p = p->next;
                                            }
                                            if (p)
                                                failed = true;
                                        }
                                        dest = dest->next;
                                        //                                    src = src->next;
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
                                if (bcsym && bcsym->sb->instantiated && allTemplateArgsSpecified(bcsym, bcsym->templateParams->next))
                                    bcsym = TemplateClassInstantiateInternal(bcsym, bcsym->templateParams->next, false);
                            }
                        }
                    }
                }
                else if (MATCHKW(lex, lt))
                {
                    errorstr(ERR_NOT_A_TEMPLATE, bcsym ? bcsym->name : name[0] ? name : "unknown");
                }
                if (bcsym && bcsym->tp->templateParam && bcsym->tp->templateParam->p->packed)
                {
                    if (bcsym->tp->templateParam->p->type != kw_typename)
                        error(ERR_NEED_PACKED_TEMPLATE_OF_TYPE_CLASS);
                    else
                    {
                        TEMPLATEPARAMLIST* templateParams = bcsym->tp->templateParam->p->byPack.pack;
                        while (templateParams)
                        {
                            if (!isstructured(templateParams->p->byClass.val))
                            {
                                errorcurrent(ERR_STRUCTURED_TYPE_EXPECTED_IN_PACKED_TEMPLATE_PARAMETER);
                            }
                            else
                            {
                                *bc = innerBaseClass(declsym, templateParams->p->byClass.val->sp, isvirtual, currentAccess);
                                if (*bc)
                                    bc = &(*bc)->next;
                            }
                            templateParams = templateParams->next;
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
                else if (bcsym && bcsym->tp->templateParam && !bcsym->tp->templateParam->p->packed)
                {
                    if (bcsym->tp->templateParam->p->type != kw_typename)
                        error(ERR_CLASS_TEMPLATE_PARAMETER_EXPECTED);
                    else
                    {
                        TYPE* tp = bcsym->tp->templateParam->p->byClass.val;
                        if (tp)
                        {
                            tp = basetype(tp);
                            if (tp->type == bt_templateselector)
                            {
                                SYMBOL* sym = tp->sp->sb->templateSelector->next->sp;
                                TEMPLATESELECTOR* lst = tp->sp->sb->templateSelector->next->next;
                                while (lst && sym)
                                {
                                    PerformDeferredInitialization(sym->tp, funcsp);
                                    sym = search(lst->name, sym->tp->syms);
                                    lst = lst->next;
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
                            *bc = innerBaseClass(declsym, tp->sp, isvirtual, currentAccess);
                            if (*bc)
                                bc = &(*bc)->next;
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
                    *bc = innerBaseClass(declsym, bcsym, isvirtual, currentAccess);
                    if (*bc)
                        bc = &(*bc)->next;
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
            if (!done)
                ParseAttributeSpecifiers(&lex, funcsp, true);
        } while (!done);
        dropStructureDeclaration();
        lst = declsym->sb->baseClasses;
        while (lst)
        {
            if (!isExpressionAccessible(nullptr, lst->cls, nullptr, nullptr, false))
            {
                bool err = true;
                BASECLASS* lst2 = declsym->sb->baseClasses;
                while (lst2)
                {
                    if (lst2 != lst)
                    {
                        if (isAccessible(lst2->cls, lst2->cls, lst->cls, nullptr, ac_protected, false))
                        {
                            err = false;
                            break;
                        }
                    }
                    lst2 = lst2->next;
                }
                if (err)
                    errorsym(ERR_CANNOT_ACCESS, lst->cls);
            }
            lst = lst->next;
        }
        return lex;
    }
    static bool hasPackedTemplate(TYPE* tp)
    {
        SYMLIST* hr;
        SYMBOL* sym;

        switch (tp->type)
        {
            case bt_typedef:
                break;
            case bt_aggregate:
                hr = tp->syms->table[0];
                sym = hr->p;
                tp = sym->tp;
                /* fall through */
            case bt_func:
            case bt_ifunc:
                if (hasPackedTemplate(tp->btp))
                    return true;
                if (tp->syms)
                {
                    hr = tp->syms->table[0];
                    while (hr)
                    {
                        sym = hr->p;
                        if (hasPackedTemplate(sym->tp))
                            return true;
                        hr = hr->next;
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
                return tp->templateParam->p->packed;
            default:
                diag("hasPackedTemplateParam: unknown type");
                break;
        }
        return 0;
    }
    void checkPackedType(SYMBOL* sym)
    {
        /*
        if (!hasPackedTemplate(tp))
        {
            error(ERR_PACK_SPECIFIER_REQUIRES_PACKED_TEMPLATE_PARAMETER);
        }
        else
        */
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
                    TEMPLATEPARAMLIST* tpl = exp1->v.func->templateParams;
                    while (tpl)
                    {
                        if (tpl->p->packed)
                            return true;
                        tpl = tpl->next;
                    }
                }
                INITLIST* il = exp1->v.func->arguments;
                while (il)
                {
                    if (hasPackedExpression(il->exp, useAuto))
                        return true;
                    il = il->next;
                }
                if (exp1->v.func->thisptr && hasPackedExpression(exp1->v.func->thisptr, useAuto))
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
                TEMPLATEPARAMLIST* tpl = spx->templateParams;
                while (tpl)
                {
                    if (tpl->p->packed)
                    {
                        SYMBOL* sym = clonesym(spx);
                        sym->tp = (TYPE*)Alloc(sizeof(TYPE));
                        sym->tp->type = bt_templateparam;
                        sym->tp->templateParam = tpl;
                        arg[(*count)++] = sym;
                        NormalizePacked(packedExp->v.sp->tp);
                    }
                    tpl = tpl->next;
                }
                spx = spx->sb->parentClass;
            }
        }
        else if (packedExp->type == en_func)
        {
            INITLIST* lst;
            TEMPLATEPARAMLIST* tpl = packedExp->v.func->templateParams;
            while (tpl)
            {
                if (tpl->p->packed && tpl->argsym)
                {
                    arg[(*count)++] = tpl->argsym;
                    NormalizePacked(tpl->argsym->tp);
                    break;
                }
                tpl = tpl->next;
            }
            lst = packedExp->v.func->arguments;
            while (lst)
            {
                GatherPackedVars(count, arg, lst->exp);
                lst = lst->next;
            }
        }
        else if (packedExp->type == en_templateselector)
        {
            TEMPLATEPARAMLIST* tpl = packedExp->v.templateSelector->next->templateParams;
            while (tpl)
            {
                if (tpl->p->packed && tpl->argsym)
                {
                    arg[(*count)++] = tpl->argsym;
                }
                tpl = tpl->next;
            }
        }
    }
    int CountPacks(TEMPLATEPARAMLIST* packs)
    {
        int rv = 0;
        std::stack<TEMPLATEPARAMLIST*> tps;
        while (packs)
        {
            if (packs->p->packed)
            {
                tps.push(packs->next);
            }
            else
            {
                rv++;
            }
            packs = packs->next;
            if (!packs && tps.size())
            {
                packs = tps.top();
                tps.pop();
            }
        }
        return rv;
    }
    INITLIST** expandPackedInitList(INITLIST** lptr, SYMBOL* funcsp, LEXEME* start, EXPRESSION* packedExp)
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
                SYMLIST* hr = basetype(arg[0]->sb->parent->tp)->syms->table[0];
                while (hr->p && hr->p != arg[0])
                    hr = hr->next;
                if (hr)
                {
                    while (hr)
                    {
                        SYMBOL* sym = hr->p;
                        INITLIST* p = (INITLIST*)Alloc(sizeof(INITLIST));
                        p->tp = sym->tp;
                        p->exp = varNode(en_auto, sym);
                        if (isref(p->tp))
                        {
                            p->exp = exprNode(en_l_p, p->exp, nullptr);
                            p->tp = basetype(p->tp)->btp;
                        }
                        if (!isstructured(p->tp))
                            deref(p->tp, &p->exp);
                        *lptr = p;
                        lptr = &(*lptr)->next;
                        hr = hr->next;
                    }
                }
            }
            else
            {
                int i;
                int n = CountPacks(arg[0]->tp->templateParam->p->byPack.pack);
                /*
                for (i=1; i < count; i++)
                {
                    if (CountPacks(arg[i]->tp->templateParam->p->byPack.pack) != n)
                    {
                        CountPacks(arg[i]->tp->templateParam->p->byPack.pack);
                        error(ERR_PACK_SPECIFIERS_SIZE_MISMATCH);
                        break;
                    }
                }
                */
                if (n > 1 || !packedExp->v.func->arguments || packedExp->v.func->arguments->tp->type != bt_void)
                    for (i = 0; i < n; i++)
                    {
                        INITLIST* p = (INITLIST*)Alloc(sizeof(INITLIST));
                        LEXEME* lex = SetAlternateLex(start);
                        packIndex = i;
                        expression_assign(lex, funcsp, nullptr, &p->tp, &p->exp, nullptr, _F_PACKABLE);
                        SetAlternateLex(nullptr);
                        if (p->tp->type != bt_void)
                            if (p->tp)
                            {
                                *lptr = p;
                                lptr = &(*lptr)->next;
                            }
                    }
            }
        }
        expandingParams--;
        packIndex = oldPack;
        return lptr;
    }
    static int GetBaseClassList(const char* name, SYMBOL* cls, BASECLASS* bc, BASECLASS** result)
    {
        (void)cls;
        int n = 0;
        char str[1024];
        char* clslst[100];
        char *p = str, *c;
        int ccount = 0;
        strcpy(str, name);
        while ((c = strstr(p, "::")))
        {
            clslst[n++] = p;
            p = c;
            *p = 0;
            p += 2;
        }
        clslst[n++] = p;

        while (bc)
        {
            if (!bc->isvirtual)
            {
                SYMBOL* parent = bc->cls;
                int i;
                if (parent->tp->type == bt_typedef)
                    parent = basetype(parent->tp)->sp;
                for (i = n - 1; i >= 0 && parent; i--, parent = parent->sb->parentClass ? parent->sb->parentClass : parent->sb->parentNameSpace)
                    if (strcmp(parent->name, clslst[i]))
                        break;
                if (i < 0 || (i == 0 && parent == nullptr && clslst[0][0] == '\0'))
                {
                    result[ccount++] = bc;
                }
            }
            bc = bc->next;
        }
        return ccount;
    }
    static int GetVBaseClassList(const char* name, SYMBOL* cls, VBASEENTRY* vbase, VBASEENTRY** result)
    {
        (void)cls;
        int n = 0;
        char str[1024];
        char* clslst[100];
        char *p = str, *c;
        int vcount = 0;
        strcpy(str, name);
        while ((c = strstr(p, "::")))
        {
            clslst[n++] = p;
            p = c;
            *p = 0;
            p += 2;
        }
        clslst[n++] = p;

        while (vbase)
        {
            SYMBOL* parent = vbase->cls;
            int i;
            if (parent->tp->type == bt_typedef)
                parent = basetype(parent->tp)->sp;
            for (i = n - 1; i >= 0 && parent; i--, parent = parent->sb->parentClass ? parent->sb->parentClass : parent->sb->parentNameSpace)
                if (strcmp(parent->name, clslst[i]))
                    break;
            if (i < 0 || (i == 0 && parent == nullptr && clslst[0][0] == '\0'))
            {
                result[vcount++] = vbase;
            }
            vbase = vbase->next;
        }
        return vcount;
    }
    MEMBERINITIALIZERS* expandPackedBaseClasses(SYMBOL* cls, SYMBOL* funcsp, MEMBERINITIALIZERS** init, BASECLASS* bc,
                                                VBASEENTRY* vbase)
    {
        MEMBERINITIALIZERS* linit = *init;
        int basecount = 0, vbasecount = 0;
        BASECLASS* baseEntries[200];
        VBASEENTRY* vbaseEntries[200];
        basecount = GetBaseClassList(linit->name, cls, bc, baseEntries);
        if (!basecount)
            vbasecount = GetVBaseClassList(linit->name, cls, vbase, vbaseEntries);
        if (!basecount && !vbasecount)
        {
            // already evaluated to not having a base class
            init = &(*init)->next;
        }
        else if ((basecount && !baseEntries[0]->cls->sb->templateLevel) || (vbasecount && !vbaseEntries[0]->cls->sb->templateLevel))
        {
            init = &(*init)->next;
            errorsym(ERR_NOT_A_TEMPLATE, linit->sp);
        }
        else
        {
            LEXEME* lex = SetAlternateLex(linit->initData);
            *init = (*init)->next;
            if (MATCHKW(lex, lt))
            {
                // at this point we've already created the independent base classes
                // but the initdata has the argument list, so get it out of the way
                // and also count the number of packs to see if it matches the number of templates..
                int n = -1;
                TEMPLATEPARAMLIST *lst = nullptr, *pack;
                LEXEME* arglex = GetTemplateArguments(lex, funcsp, linit->sp, &lst);
                SetAlternateLex(nullptr);
                pack = lst;
                while (pack)
                {
                    if (pack->p->packed)
                    {
                        int n1 = CountPacks(pack->p->byPack.pack);
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
                    pack = pack->next;
                }
                if (n != -1)
                {
                    int i;
                    // the presumption is that the number of packed vars will be the same as the number
                    // of instantiated templates...
                    //
                    // I have also seen n be 0 during parsing...
                    for (i = 0; i < n; i++)
                    {
                        int oldPack = packIndex;
                        SYMBOL* baseSP = basecount ? baseEntries[i]->cls : vbaseEntries[i]->cls;
                        MEMBERINITIALIZERS* added = (MEMBERINITIALIZERS*)Alloc(sizeof(MEMBERINITIALIZERS));
                        bool done = false;
                        lex = SetAlternateLex(arglex);
                        packIndex = i;
                        added->name = linit->name;
                        added->next = *init;
                        *init = added;
                        init = &added->next;
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
                            INITIALIZER** xinit = &added->init;
                            added->sp = sym;
                            shim.arguments = nullptr;
                            getMemberInitializers(lex, funcsp, &shim, MATCHKW(lex, openpa) ? closepa : end, false);
                            while (shim.arguments)
                            {
                                *xinit = (INITIALIZER*)(INITIALIZER*)Alloc(sizeof(INITIALIZER));
                                (*xinit)->basetp = shim.arguments->tp;
                                (*xinit)->exp = shim.arguments->exp;
                                xinit = &(*xinit)->next;
                                shim.arguments = shim.arguments->next;
                            }
                        }
                        SetAlternateLex(nullptr);
                        packIndex = oldPack;
                    }
                }
            }
            else
            {
                lex = SetAlternateLex(nullptr);
                error(ERR_NEED_SPECIALIZATION_PARAMETERS);
            }
        }
        return *init;
    }
    void expandPackedMemberInitializers(SYMBOL* cls, SYMBOL* funcsp, TEMPLATEPARAMLIST* templatePack, MEMBERINITIALIZERS** p,
                                        LEXEME* start, INITLIST* list)
    {
        int n = CountPacks(templatePack);
        MEMBERINITIALIZERS* orig = *p;
        *p = (*p)->next;
        if (n)
        {
            int count = 0;
            int i;
            int oldPack;
            SYMBOL* arg[300];
            TEMPLATEPARAMLIST* pack = templatePack;
            while (pack)
            {
                if (!isstructured(pack->p->byClass.val))
                {
                    error(ERR_STRUCTURED_TYPE_EXPECTED_IN_PACKED_TEMPLATE_PARAMETER);
                    return;
                }
                pack = pack->next;
            }
            while (list)
            {
                GatherPackedVars(&count, arg, list->exp);
                list = list->next;
            }
            for (i = 0; i < count; i++)
            {
                if (CountPacks(arg[i]->tp->templateParam->p->byPack.pack) != n)
                {
                    error(ERR_PACK_SPECIFIERS_SIZE_MISMATCH);
                    break;
                }
            }
            oldPack = packIndex;
            for (i = 0; i < n; i++)
            {
                LEXEME* lex = SetAlternateLex(start);
                MEMBERINITIALIZERS* mi = (MEMBERINITIALIZERS*)Alloc(sizeof(MEMBERINITIALIZERS));
                TYPE* tp = templatePack->p->byClass.val;
                BASECLASS* bc = cls->sb->baseClasses;
                int offset = 0;
                int vcount = 0, ccount = 0;
                *mi = *orig;
                mi->sp = nullptr;
                packIndex = i;
                mi->name = templatePack->p->byClass.val->sp->name;
                while (bc)
                {
                    if (!strcmp(bc->cls->name, mi->name))
                    {
                        if (bc->isvirtual)
                            vcount++;
                        else
                            ccount++;
                        mi->sp = bc->cls;
                        offset = bc->offset;
                    }
                    bc = bc->next;
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
                        INITIALIZER** xinit = &mi->init;
                        mi->sp = sym;
                        lex = SetAlternateLex(mi->initData);
                        shim.arguments = nullptr;
                        getMemberInitializers(lex, funcsp, &shim, MATCHKW(lex, openpa) ? closepa : end, false);
                        SetAlternateLex(nullptr);
                        while (shim.arguments)
                        {
                            *xinit = (INITIALIZER*)(INITIALIZER*)Alloc(sizeof(INITIALIZER));
                            (*xinit)->basetp = shim.arguments->tp;
                            (*xinit)->exp = shim.arguments->exp;
                            xinit = &(*xinit)->next;
                            shim.arguments = shim.arguments->next;
                        }
                    }
                }
                else
                {
                    mi->sp = nullptr;
                }
                if (mi->sp)
                {
                    mi->next = *p;
                    *p = mi;
                    p = &(*p)->next;
                }
                else
                {
                    errorstrsym(ERR_NOT_A_MEMBER_OR_BASE_CLASS, mi->name, cls);
                }
                SetAlternateLex(nullptr);
                templatePack = templatePack->next;
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
            SYMLIST* hr = basetype(sp->tp)->syms->table[0];
            if (!hr)
                return;
            if (!asFriend && getStructureDeclaration())  // nonstatic member
            {
                if (sp->sb->operatorId == CI_CAST)
                {
                    // needs no argument
                    SYMBOL* sym = hr->p;
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
                            if (hr && hr->next)
                            {
                                errorstr(ERR_OPERATOR_NEEDS_ZERO_OR_ONE_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                            }
                            if (!hr)
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
                            sym = hr->p;
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
                            sym = hr->p;
                            if (sym->tp->type == bt_void || hr->next)
                            {
                                errorstr(ERR_OPERATOR_NEEDS_ONE_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                            }
                            break;
                        case autoinc:
                        case autodec:
                            if (hr->next)
                            {
                                errorstr(ERR_OPERATOR_NEEDS_ZERO_OR_ONE_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                            }
                            sym = hr->p;
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
                            sym = hr->p;
                            if (sym->tp->type == bt_void || hr->next)
                            {
                                errorstr(ERR_OPERATOR_NEEDS_ONE_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                            }
                            break;

                        case openbr:
                            // needs one argument:
                            sym = hr->p;
                            if (sym->tp->type == bt_void || hr->next)
                            {
                                errorstr(ERR_OPERATOR_NEEDS_ONE_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                            }
                            break;
                        case openpa:
                            // anything goes
                            break;
                        case pointsto:
                            // needs no arguments
                            sym = hr->p;
                            if (sym->tp->type != bt_void)
                            {
                                errorstr(ERR_OPERATOR_NEEDS_NO_PARAMETERS, overloadXlateTab[sp->sb->operatorId]);
                            }
                            else
                            {
                                TYPE* tp = basetype(sp->tp)->btp;
                                if (!ispointer(tp) && !isref(tp))
                                {
                                    if (!templateNestingCount)
                                        errorstr(ERR_OPERATOR_RETURN_REFERENCE_OR_POINTER, overloadXlateTab[sp->sb->operatorId]);
                                }
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
                    else
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
                        sym = hr->p;
                        if (sym->tp->type == bt_void || (hr->next && hr->next->next))
                        {
                            errorstr(ERR_OPERATOR_NEEDS_ONE_OR_TWO_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                        }
                        else if (!classOrEnumParam(hr->p) && (!hr->next || !classOrEnumParam(hr->next->p)))
                        {
                            if (!templateNestingCount)
                                errorstr(ERR_OPERATOR_NEEDS_A_CLASS_OR_ENUMERATION_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                        }
                        break;
                    case notx:
                    case complx:
                        // needs one arg of class or enum type
                        sym = hr->p;
                        if (sym->tp->type == bt_void || hr->next)
                        {
                            errorstr(ERR_OPERATOR_NEEDS_ONE_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                        }
                        else if (!classOrEnumParam(hr->p))
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
                        if (!hr || !hr->next || hr->next->next)
                        {
                            errorstr(ERR_OPERATOR_NEEDS_TWO_PARAMETERS, overloadXlateTab[sp->sb->operatorId]);
                        }
                        else if (!classOrEnumParam(hr->p) && !classOrEnumParam((SYMBOL*)(hr->next->p)))
                        {
                            if (!templateNestingCount)
                                errorstr(ERR_OPERATOR_NEEDS_A_CLASS_OR_ENUMERATION_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                        }
                        break;
                    case autoinc:
                    case autodec:
                        // needs one or two args, first of class or enum type
                        // if second is present int type
                        sym = hr->p;
                        if (sym->tp->type == bt_void || (hr->next && hr->next->next))
                        {
                            errorstr(ERR_OPERATOR_NEEDS_ONE_OR_TWO_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                        }
                        else if (!classOrEnumParam(sym))
                        {
                            if (!templateNestingCount)
                                errorstr(ERR_OPERATOR_NEEDS_A_CLASS_OR_ENUMERATION_PARAMETER, overloadXlateTab[sp->sb->operatorId]);
                        }
                        if (hr && hr->next)
                        {
                            sym = (SYMBOL*)(hr->next->p);
                            if (sym->tp->type != bt_int)
                            {
                                errorstr(ERR_OPERATOR_NEEDS_SECOND_PARAMETER_OF_TYPE_INT, overloadXlateTab[sp->sb->operatorId]);
                            }
                        }
                        break;
                    case quot:
                        if (hr && hr->next)
                        {
                            if (hr->next->next)
                            {
                                errorsym(ERR_OPERATOR_LITERAL_INVALID_PARAMETER_LIST, sp);
                            }
                            else
                            {
                                // two args
                                TYPE* tpl = hr->p->tp;
                                TYPE* tpr = (hr->next->p)->tp;
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
                        }
                        else
                        {
                            // one arg
                            TYPE* tp = hr->p->tp;
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
                        if (hr)
                        {
                            // any number of args, but first must be a size
                            TYPE* tp = hr->p->tp;
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
                        if (hr)
                        {
                            // one or more args, first must be a pointer
                            TYPE* tp = hr->p->tp;
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
    LEXEME* handleStaticAssert(LEXEME* lex)
    {
        if (getStructureDeclaration())
        {
            SYMBOL* sym = getStructureDeclaration();
            Optimizer::LIST* staticAssert = (Optimizer::LIST*)Alloc(sizeof(Optimizer::LIST));
            LEXEME **cur = (LEXEME**)&staticAssert->data, *last = nullptr;
            int paren = 0;
            int brack = 0;
            staticAssert->next = sym->sb->staticAsserts;
            sym->sb->staticAsserts = staticAssert;
            while (lex != nullptr)
            {
                enum e_kw kw = KW(lex);
                if (kw == openpa)
                {
                    paren++;
                }
                else if (kw == closepa)
                {
                    if (paren-- == 0 && !brack)
                    {
                        break;
                    }
                }
                else if (kw == openbr)
                {
                    brack++;
                }
                else if (kw == closebr)
                {
                    brack--;
                }
                *cur = (LEXEME*)Alloc(sizeof(LEXEME));
                if (lex->type == l_id)
                    lex->value.s.a = litlate(lex->value.s.a);
                **cur = *lex;
                (*cur)->prev = last;
                last = *cur;
                cur = &(*cur)->next;
                if (kw == semicolon)
                {
                    break;
                }
                lex = getsym();
            }
        }
        else if (!needkw(&lex, openpa))
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
            lex = expression_no_comma(lex, nullptr, nullptr, &tp, &expr, nullptr, 0);
            expr2 = (EXPRESSION*)Alloc(sizeof(EXPRESSION));
            expr2->type = en_x_bool;
            expr2->left = expr;
            optimize_for_constants(&expr2);
            if (!isarithmeticconst(expr2) && !templateNestingCount)
                error(ERR_CONSTANT_VALUE_EXPECTED);
            v = expr2->v.i;

            if (!needkw(&lex, comma))
            {
                errskim(&lex, skim_closepa);
                skip(&lex, closepa);
            }
            else
            {
                if (lex->type != l_astr)
                {
                    error(ERR_NEEDSTRING);
                }
                else
                {
                    int i, pos = 0;
                    while (lex->type == l_astr)
                    {
                        Optimizer::SLCHAR* ch = (Optimizer::SLCHAR*)lex->value.s.w;
                        lex = getsym();
                        for (i = 0; i < ch->count && i + pos < sizeof(buf) - 1; i++)
                            buf[i + pos] = ch->str[i];
                        pos += i;
                    }
                    buf[pos] = 0;
                }
                if (!needkw(&lex, closepa))
                {
                    errskim(&lex, skim_closepa);
                    skip(&lex, closepa);
                }
                else if (!v && (!templateNestingCount))  // || instantiatingTemplate))
                {
                    errorstr(ERR_PURESTRING, buf);
                }
            }
        }
        return lex;
    }
    LEXEME* insertNamespace(LEXEME* lex, enum e_lk linkage, enum e_sc storage_class, bool* linked)
    {
        bool anon = false;
        char buf[256], *p;
        SYMLIST** hr;
        SYMBOL* sym;
        Optimizer::LIST* list;
        *linked = false;
        if (ISID(lex))
        {
            strcpy(buf, lex->value.s.a);
            lex = getsym();
            if (MATCHKW(lex, assign))
            {
                lex = getsym();
                if (ISID(lex))
                {
                    char buf1[512];
                    strcpy(buf1, lex->value.s.a);
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
                            TYPE* tp;
                            SYMLIST** p;
                            if (storage_class == sc_auto)
                                p = LookupName(buf, localNameSpace->valueData->syms);
                            else
                                p = LookupName(buf, globalNameSpace->valueData->syms);
                            if (p)
                            {
                                SYMBOL* sym = (SYMBOL*)(*p)->p;
                                // already exists, bug check it
                                if (sym->sb->storage_class == sc_namespacealias && sym->sb->nameSpaceValues->valueData->origname == src)
                                {
                                    if (linkage == lk_inline)
                                    {
                                        error(ERR_INLINE_NOT_ALLOWED);
                                    }
                                    lex = getsym();
                                    return lex;
                                }
                            }
                            tp = (TYPE*)(TYPE*)Alloc(sizeof(TYPE));
                            tp->type = bt_void;
                            tp->rootType = tp;
                            sym = makeID(sc_namespacealias, tp, nullptr, litlate(buf));
                            if (nameSpaceList)
                            {
                                sym->sb->parentNameSpace = (SYMBOL*)nameSpaceList->data;
                            }
                            SetLinkerNames(sym, lk_none);
                            if (storage_class == sc_auto)
                            {
                                insert(sym, localNameSpace->valueData->syms);
                                insert(sym, localNameSpace->valueData->tags);
                            }
                            else
                            {
                                insert(sym, globalNameSpace->valueData->syms);
                                insert(sym, globalNameSpace->valueData->tags);
                            }
                            sym->sb->nameSpaceValues = (NAMESPACEVALUELIST*)Alloc(sizeof(NAMESPACEVALUELIST));
                            sym->sb->nameSpaceValues->valueData = (NAMESPACEVALUEDATA*)Alloc(sizeof(NAMESPACEVALUEDATA));
                            *sym->sb->nameSpaceValues->valueData = *src->sb->nameSpaceValues->valueData;
                            sym->sb->nameSpaceValues->valueData->name = sym;  // this is to rename it with the alias e.g. for errors
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
                p = strrchr(infile, '\\');
                if (!p)
                {
                    p = strrchr(infile, '/');
                    if (!p)
                        p = infile;
                    else
                        p++;
                }
                else
                    p++;

                sprintf(anonymousNameSpaceName, "__%s__%08x", p, Utils::CRC32((unsigned char*)infile, strlen(infile)));
                while ((p = strchr(anonymousNameSpaceName, '.')) != 0)
                    *p = '_';
            }
            strcpy(buf, anonymousNameSpaceName);
        }
        if (storage_class != sc_global)
        {
            error(ERR_NO_NAMESPACE_IN_FUNCTION);
        }
        hr = LookupName(buf, globalNameSpace->valueData->syms);
        if (!hr)
        {
            TYPE* tp = (TYPE*)(TYPE*)Alloc(sizeof(TYPE));
            tp->type = bt_void;
            tp->rootType = tp;
            sym = makeID(sc_namespace, tp, nullptr, litlate(buf));
            sym->sb->nameSpaceValues = (NAMESPACEVALUELIST*)Alloc(sizeof(NAMESPACEVALUELIST));
            sym->sb->nameSpaceValues->valueData = (NAMESPACEVALUEDATA*)Alloc(sizeof(NAMESPACEVALUEDATA));
            sym->sb->nameSpaceValues->valueData->syms = CreateHashTable(GLOBALHASHSIZE);
            sym->sb->nameSpaceValues->valueData->tags = CreateHashTable(GLOBALHASHSIZE);
            sym->sb->nameSpaceValues->valueData->origname = sym;
            sym->sb->nameSpaceValues->valueData->name = sym;
            sym->sb->parentNameSpace = globalNameSpace->valueData->name;
            sym->sb->attribs.inheritable.linkage = linkage;
            if (nameSpaceList)
            {
                sym->sb->parentNameSpace = (SYMBOL*)nameSpaceList->data;
            }
            SetLinkerNames(sym, lk_none);
            insert(sym, globalNameSpace->valueData->syms);
            insert(sym, globalNameSpace->valueData->tags);
            if (anon || linkage == lk_inline)
            {
                // plop in a using directive for the anonymous namespace we are declaring
                list = (Optimizer::LIST*)Alloc(sizeof(Optimizer::LIST));
                list->data = sym;
                if (linkage == lk_inline)
                {
                    list->next = globalNameSpace->valueData->inlineDirectives;
                    globalNameSpace->valueData->inlineDirectives = list;
                }
                else
                {
                    list->next = globalNameSpace->valueData->usingDirectives;
                    globalNameSpace->valueData->usingDirectives = list;
                }
            }
        }
        else
        {
            sym = (SYMBOL*)(*hr)->p;
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

        list = (Optimizer::LIST*)Alloc(sizeof(Optimizer::LIST));
        list->next = nameSpaceList;
        list->data = sym;
        nameSpaceList = list;

        sym->sb->nameSpaceValues->next = globalNameSpace;
        globalNameSpace = sym->sb->nameSpaceValues;

        *linked = true;
        return lex;
    }
    void unvisitUsingDirectives(NAMESPACEVALUELIST* v)
    {
        Optimizer::LIST* t = v->valueData->usingDirectives;
        while (t)
        {
            SYMBOL* sym = (SYMBOL*)t->data;
            sym->sb->visited = false;
            unvisitUsingDirectives(sym->sb->nameSpaceValues);
            t = t->next;
        }
        t = v->valueData->inlineDirectives;
        while (t)
        {
            SYMBOL* sym = (SYMBOL*)t->data;
            sym->sb->visited = false;
            unvisitUsingDirectives(sym->sb->nameSpaceValues);
            t = t->next;
        }
    }
    static void InsertTag(SYMBOL* sym, enum e_sc storage_class, bool allowDups)
    {
        HASHTABLE* table;
        SYMBOL* ssp = getStructureDeclaration();
        SYMBOL* sp1 = nullptr;
        if (ssp && (storage_class == sc_member || storage_class == sc_mutable || storage_class == sc_type))
        {
            table = ssp->tp->tags;
        }
        else if (storage_class == sc_auto || storage_class == sc_register || storage_class == sc_parameter ||
                 storage_class == sc_localstatic)
            table = localNameSpace->valueData->tags;
        else
            table = globalNameSpace->valueData->tags;
        if (allowDups)
            sp1 = search(sym->name, table);
        if (!allowDups || !sp1 || (sym != sp1 && sym->sb->mainsym && sym->sb->mainsym != sp1->sb->mainsym))
            insert(sym, table);
    }
    LEXEME* insertUsing(LEXEME* lex, SYMBOL** sp_out, enum e_ac access, enum e_sc storage_class, bool inTemplate, bool hasAttributes)
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
                        Optimizer::LIST* t = globalNameSpace->valueData->usingDirectives;
                        while (t)
                        {
                            if (t->data == sp)
                                break;
                            t = t->next;
                        }
                        if (!t)
                        {
                            Optimizer::LIST* l = (Optimizer::LIST*)Alloc(sizeof(Optimizer::LIST));
                            l->data = sp;
                            if (storage_class == sc_auto)
                            {
                                l->next = localNameSpace->valueData->usingDirectives;
                                localNameSpace->valueData->usingDirectives = l;
                            }
                            else
                            {
                                l->next = globalNameSpace->valueData->usingDirectives;
                                globalNameSpace->valueData->usingDirectives = l;
                            }
    #ifdef PARSER_ONLY
                            if (lex)
                                CompletionCompiler::ccInsertUsing(sp, nameSpaceList ? (SYMBOL*)nameSpaceList->data : nullptr, lex->file, lex->line);
    #endif
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
                LEXEME* idsym = lex;
                lex = getsym();
                ParseAttributeSpecifiers(&lex, nullptr, true);
                if (MATCHKW(lex, assign))
                {
                    TYPE* tp = nullptr;
                    SYMBOL* sp;
                    lex = getsym();
                    lex = get_type_id(lex, &tp, nullptr, sc_cast, false, true);
                    if (!tp)
                        tp = &stdint;
                    checkauto(tp, ERR_AUTO_NOT_ALLOWED_IN_USING_STATEMENT);
                    sp = makeID(sc_typedef, tp, nullptr, litlate(idsym->value.s.a));
                    if (inTemplate)
                    {
                        TYPE* tp1 = (TYPE*)(TYPE*)Alloc(sizeof(TYPE));
                        tp1->type = bt_typedef;
                        tp1->btp = tp;
                        tp1->sp = sp;
                        UpdateRootTypes(tp1);
                        sp->tp = tp1;
                        sp->sb->templateLevel = templateNestingCount;
                        sp->templateParams = TemplateGetParams(sp);
                    }
                    if (storage_class == sc_member)
                        sp->sb->parentClass = getStructureDeclaration();
                    SetLinkerNames(sp, lk_cdecl);
                    InsertSymbol(sp, storage_class, lk_cdecl, false);
                    if (sp_out)
                        *sp_out = sp;
                    return lex;
                }
                else
                {
                    lex = backupsym();
                }
            }
            lex = nestedSearch(lex, &sp, nullptr, nullptr, nullptr, nullptr, false, sc_global, true, false);
            if (sp)
            {
                if (!templateNestingCount)
                {
                    if (sp->sb->storage_class == sc_overloads)
                    {
                        SYMLIST** hr = sp->tp->syms->table;
                        while (*hr)
                        {
                            SYMBOL *ssp = getStructureDeclaration(), *ssp1;
                            SYMBOL* sp = (SYMBOL*)(*hr)->p;
                            SYMBOL* sp1 = clonesym(sp);
                            sp1->sb->wasUsing = true;
                            ssp1 = sp1->sb->parentClass;
                            if (ssp && ismember(sp1))
                                sp1->sb->parentClass = ssp;
                            sp1->sb->mainsym = sp;
                            // while (sp1->sb->mainsym->sb->mainsym)
                            //    sp1->sb->mainsym = sp1->sb->mainsym->sb->mainsym;
                            sp1->sb->access = access;
                            InsertSymbol(sp1, storage_class, sp1->sb->attribs.inheritable.linkage, true);
                            InsertInline(sp1);
                            sp1->sb->parentClass = ssp1;
                            hr = &(*hr)->next;
                        }
                        if (isTypename)
                            error(ERR_TYPE_NAME_EXPECTED);
                    }
                    else
                    {
                        SYMBOL *ssp = getStructureDeclaration(), *ssp1;
                        SYMBOL* sp1 = clonesym(sp);
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
                lex = getsym();
            }
        }
        return lex;
    }
    static void balancedAttributeParameter(LEXEME** lex)
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
                TYPE* tp1 = (TYPE*)Alloc(sizeof(TYPE));
                tp1->type = bt_pointer;
                tp1->rootType = tp1;
                tp1->size = sym->sb->attribs.inheritable.vectorSize;
                tp1->array = true;
                tp1->btp = tp;
                tp = tp1;
            }
            else
            {
                error(ERR_INVALID_VECTOR_TYPE);
            }
        }
        if (sym->sb->attribs.inheritable.cleanup && sym->sb->storage_class == sc_auto)
        {
            FUNCTIONCALL* fc = (FUNCTIONCALL*)Alloc(sizeof(FUNCTIONCALL));
            fc->arguments = (INITLIST*)Alloc(sizeof(INITLIST));
            fc->arguments->tp = &stdpointer;
            fc->arguments->exp = varNode(en_auto, sym);
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
    void ParseOut__attribute__(LEXEME** lex, SYMBOL* funcsp)
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
                            {"stdcall", 25 }
                
                        };
                        std::string name;
                        if (ISID(*lex))
                            name = (*lex)->value.s.a;
                        else
                            name = (*lex)->kw->name;
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
                                        if ((*lex)->type == l_astr)
                                        {
                                            char buf[1024];
                                            int i;
                                            Optimizer::SLCHAR* xx = (Optimizer::SLCHAR*)(*lex)->value.s.w;
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
                                    if (needkw(lex, openpa))
                                    {
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
                                            SYMBOL* sym = gsearch((*lex)->value.s.a);
                                            if (sym)
                                            {
                                                if (sym->tp->type == bt_aggregate)
                                                    if (basetype(sym->tp)->syms->table[0] && !basetype(sym->tp)->syms->table[0]->next)
                                                        sym = basetype(sym->tp)->syms->table[0]->p;
                                                if (isfunction(sym->tp) && isvoid(basetype(sym->tp)->btp))
                                                {
                                                    auto arg = basetype(sym->tp)->syms->table[0];
                                                    if (!arg || arg == (void*)-1 || isvoid(arg->p->tp) ||
                                                        (isvoidptr(arg->p->tp) && !arg->next))
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
                                                errorstr(ERR_UNDEFINED_IDENTIFIER, (*lex)->value.s.a);
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
                                            SYMBOL* sym = gsearch((*lex)->value.s.a);
                                            if (sym)
                                            {
                                                basisAttribs.uninheritable.copyFrom = sym;
                                            }
                                            else
                                            {
                                                errorstr(ERR_UNDEFINED_IDENTIFIER, (*lex)->value.s.a);
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
                                        if ((*lex)->type == l_astr)
                                        {
                                            char buf[1024];
                                            int i;
                                            Optimizer::SLCHAR* xx = (Optimizer::SLCHAR*)(*lex)->value.s.w;
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
                                case 25: // stdcall
                                    if (basisAttribs.inheritable.linkage != lk_none)
                                        error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                    basisAttribs.inheritable.linkage = lk_stdcall;
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
    bool ParseAttributeSpecifiers(LEXEME** lex, SYMBOL* funcsp, bool always)
    {
        (void)always;
        bool rv = false;
        if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.prm_c1x)
        {
            while (MATCHKW(*lex, kw_alignas) || MATCHKW(*lex, kw__attribute) || (Optimizer::cparams.prm_cplusplus && MATCHKW(*lex, openbr)))
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
                        if (startOfType(*lex, false))
                        {
                            TYPE* tp = nullptr;
                            *lex = get_type_id(*lex, &tp, funcsp, sc_cast, false, true);

                            if (!tp)
                            {
                                error(ERR_TYPE_NAME_EXPECTED);
                            }
                            else if (tp->type == bt_templateparam)
                            {
                                if (tp->templateParam->p->type == kw_typename)
                                {
                                    if (tp->templateParam->p->packed)
                                    {
                                        TEMPLATEPARAMLIST* packs = tp->templateParam->p->byPack.pack;
                                        if (!MATCHKW(*lex, ellipse))
                                        {
                                            error(ERR_PACK_SPECIFIER_REQUIRED_HERE);
                                        }
                                        else
                                        {
                                            *lex = getsym();
                                        }
                                        while (packs)
                                        {
                                            int v = getAlign(sc_global, packs->p->byClass.val);
                                            if (v > align)
                                                align = v;
                                            packs = packs->next;
                                        }
                                    }
                                    else
                                    {
                                        // it will only get here while parsing the template...
                                        // when generating the instance the class member will already be
                                        // filled in so it will get to the below...
                                        if (tp->templateParam->p->byClass.val)
                                            align = getAlign(sc_global, tp->templateParam->p->byClass.val);
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
                            else if (!isintconst(exp))
                                error(ERR_CONSTANT_VALUE_EXPECTED);
                            align = exp->v.i;
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
                                else if (!strcmp((*lex)->value.s.a, occNamespace.c_str()))
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
                                            static const std::unordered_map<std::string, int> occCPPStyleAttribNames = {
                                                {"zstring", 23}  // non-gcc, added to support nonstring
                                            };
                                            std::string name = (*lex)->value.s.a;
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
                                else if (!strcmp((*lex)->value.s.a, gccNamespace.c_str()))
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
                                            // note: these are only the namespaced names listed, the __attribute__ names are unlisted here as they don't
                                            // exist in GCC and we want ours to follow theirs for actual consistency reasons.
                                            static const std::unordered_map<std::string, int> gccCPPStyleAttribNames = {
                                                {"alloc_size", 4},  // implement by ignoring one or two args
                                                                    //                    { "common", 6 }, // no args, decide whether to support
                                                                    //                    { "nocommon", 7 }, // no args, decide whether to support
                                                                    //                    { "section", 12 }, // one argument, the section name
                                                //                    { "tls_model", 13 }, // one arg, the model.   Probably shouldn't support
                                                //                    { "visibility", 17 }, // one arg, 'default' ,'hidden', 'internal', 'protected.   don't
                                                //                    support for now as requires linker changes. { "weak", 18 }, // not supporting {
                                                //                    "selectany", 21 },  // requires linker support { "shared", 22 },
                                                {"dllexport", 25},
                                                {"dllimport", 26},
                                                {"stdcall", 27 }
                                            };
                                            std::string name = (*lex)->value.s.a;
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
                                    if (!strcmp((*lex)->value.s.a, "noreturn"))
                                    {
                                        *lex = getsym();
                                        special = true;
                                        if (funcsp->sb->attribs.inheritable.linkage3 != lk_none)
                                            error(ERR_TOO_MANY_LINKAGE_SPECIFIERS);
                                        funcsp->sb->attribs.inheritable.linkage3 = lk_noreturn;
                                    }
                                    else if (!strcmp((*lex)->value.s.a, "carries_dependency"))
                                    {
                                        *lex = getsym();
                                        special = true;
                                    }
                                    else
                                    {
                                        if (!strcmp((*lex)->value.s.a, "deprecated"))
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
                                if (MATCHKW(*lex, openpa))
                                {
                                    if (special)
                                        error(ERR_NO_ATTRIBUTE_ARGUMENT_CLAUSE_HERE);
                                    *lex = getsym();
                                    if (basisAttribs.uninheritable.deprecationText)
                                    {
                                        if ((*lex)->type == l_astr)
                                        {
                                            char buf[1024];
                                            int i;
                                            Optimizer::SLCHAR* xx = (Optimizer::SLCHAR*)(*lex)->value.s.w;
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
    static bool hasNoBody(STATEMENT* stmt)
    {
        while (stmt)
        {
            if (stmt->type != st_line && stmt->type != st_varstart && stmt->type != st_dbgblock)
                return false;
            if (stmt->type == st_block && !hasNoBody(stmt))
                return false;
            stmt = stmt->next;
        }
        return true;
    }
    bool isConstexprConstructor(SYMBOL* sym)
    {
        SYMLIST* hr;
        if (sym->sb->constexpression)
            return true;
        if (!sym->sb->deleted && !sym->sb->defaulted && !hasNoBody(sym->sb->inlineFunc.stmt))
            return false;
        hr = sym->sb->parentClass->tp->syms->table[0];
        while (hr)
        {
            SYMBOL* sp = hr->p;
            if (ismemberdata(sp) && !sp->sb->init)
            {
                MEMBERINITIALIZERS* memberInit = sym->sb->memberInitializers;
                while (memberInit)
                {
                    if (!strcmp(memberInit->name, sp->name))
                        break;
                    memberInit = memberInit->next;
                }
                if (!memberInit)
                    return false;
            }
            hr = hr->next;
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
            SYMLIST* hr;
            SYMBOL *cpy, *mv;
            BASECLASS* bc;
            tp = basetype(tp);
            if (sym->sb->trivialCons)
                return true;
            tp = PerformDeferredInitialization(tp, nullptr);
            sym1 = search(overloadNameTab[CI_DESTRUCTOR], tp->syms);
            if (sym1 && !((SYMBOL*)sym1->tp->syms->table[0]->p)->sb->defaulted)
                return false;
            sym1 = search(overloadNameTab[CI_CONSTRUCTOR], tp->syms);
            cpy = getCopyCons(sym, false);
            mv = getCopyCons(sym, true);
            hr = sym1->tp->syms->table[0];
            while (hr)
            {
                sym1 = hr->p;
                if (sym1 != cpy && sym1 != mv && isConstexprConstructor(sym1))
                    break;
                hr = hr->next;
            }
            if (!hr)
                return false;
            hr = tp->syms->table[0];
            while (hr)
            {
                sym1 = hr->p;
                if (sym1->sb->storage_class == sc_member && !isfunction(sym1->tp))
                    if (!constArgValid(sym1->tp))
                        return false;
                hr = hr->next;
            }
            bc = sym->sb->baseClasses;
            while (bc)
            {
                if (!constArgValid(bc->cls->tp))
                    return false;
                bc = bc->next;
            }
            return true;
        }
        return true;
    }
    bool MatchesConstFunction(SYMBOL* sym)
    {
        SYMLIST* hr;
        if (sym->sb->storage_class == sc_virtual)
            return false;
        if (!constArgValid(basetype(sym->tp)->btp))
            return false;
        hr = basetype(sym->tp)->syms->table[0];
        while (hr)
        {
            SYMBOL* sym = hr->p;
            if (sym->tp->type != bt_void && !constArgValid(sym->tp))
                return false;
            hr = hr->next;
        }
        return true;
    }
    LEXEME* getDeclType(LEXEME* lex, SYMBOL* funcsp, TYPE** tn)
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
            (*tn) = (TYPE*)(TYPE*)Alloc(sizeof(TYPE));
            (*tn)->type = bt_auto;
            (*tn)->decltypeauto = true;
            (*tn)->decltypeautoextended = extended;
        }
        else
        {
            lex = expression_no_check(lex, nullptr, nullptr, &(*tn), &exp, _F_SIZEOF);
            if ((*tn) && (*tn)->type == bt_aggregate && exp->type == en_func)
            {
                SYMLIST* hr = (*tn)->syms->table[0];
                if (hr->next)
                    errorsym2(ERR_AMBIGUITY_BETWEEN, hr->p, hr->next->p);
                exp->v.func->sp = hr->p;
                if (hasAmpersand)
                {
                    (*tn) = (TYPE*)(TYPE*)Alloc(sizeof(TYPE));
                    if (ismember(exp->v.func->sp))
                    {
                        (*tn)->type = bt_memberptr;
                        (*tn)->sp = exp->v.func->sp->sb->parentClass;
                    }
                    else
                    {
                        (*tn)->type = bt_pointer;
                        (*tn)->size = getSize(bt_pointer);
                    }
                    (*tn)->btp = exp->v.func->functp = exp->v.func->sp->tp;
                    (*tn)->rootType = (*tn);
                }
                else
                {
                    (*tn) = exp->v.func->functp = exp->v.func->sp->tp;
                }
            }
            if ((*tn))
            {
                optimize_for_constants(&exp);
                if (templateNestingCount && !instantiatingTemplate)
                {
                    TYPE* tp2 = (TYPE*)Alloc(sizeof(TYPE));
                    tp2->type = bt_templatedecltype;
                    tp2->rootType = tp2;
                    tp2->templateDeclType = exp;
                    (*tn) = tp2;
                }
            }
            exp2 = exp;
            if (!(*tn))
            {
                error(ERR_IDENTIFIER_EXPECTED);
                errskim(&lex, skim_semi_declare);
                return lex;
            }
            if (extended && lvalue(exp) && exp->left->type == en_auto)
            {
                if (!lambdas && xvalue(exp))
                {
                    TYPE* tp2 = (TYPE*)Alloc(sizeof(TYPE));
                    if (isref((*tn)))
                        (*tn) = basetype((*tn))->btp;
                    tp2->type = bt_rref;
                    tp2->size = getSize(bt_pointer);
                    tp2->btp = (*tn);
                    tp2->rootType = tp2;
                    (*tn) = tp2;
                }
                else if (lvalue(exp))
                {
                    TYPE* tp2 = (TYPE*)Alloc(sizeof(TYPE));
                    if (isref((*tn)))
                        (*tn) = basetype((*tn))->btp;
                    if (lambdas && !lambdas->isMutable)
                    {
                        tp2->type = bt_const;
                        tp2->size = (*tn)->size;
                        tp2->btp = (*tn);
                        tp2->rootType = (*tn)->rootType;
                        (*tn) = tp2;
                        tp2 = (TYPE*)Alloc(sizeof(TYPE));
                    }
                    tp2->type = bt_lref;
                    tp2->size = getSize(bt_pointer);
                    tp2->btp = (*tn);
                    tp2->rootType = (*tn)->rootType;
                    (*tn) = tp2;
                }
            }
        }
        //    if (!MATCHKW(lex, closepa))
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
                if (tp1->btp == basetype(tp1->btp))
                {
                    tp1->btp = tp1->btp->btp;
                    UpdateRootTypes(tp_in);
                }
                else
                {
                    tp1 = tp1->btp;
                }
            }
        }
    }
}