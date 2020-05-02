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

/*
  * iexpr.c
  *
  * routies to take an enode list and generate icode
  */
#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"
#include "assert.h"
#include "ppPragma.h"
#include <string>
#include <vector>
#include <algorithm>
#include "PreProcessor.h"
#include "ccerr.h"
#include "ildata.h"
#include "occparse.h"
#include "help.h"
#include "floatconv.h"
#include "memory.h"
#include "OptUtils.h"
#include "beinterf.h"

void SymbolManager::clear()
{
    if (architecture != ARCHITECTURE_MSIL || (cparams.prm_compileonly && !cparams.prm_asmfile))
        symbols.clear(); globalSymbols.clear();
}

const char* beDecorateSymName(SYMBOL* sym)
{
    const char* q;
    if (sym->sb->attribs.uninheritable.alias)
    {
        return litlate(sym->sb->attribs.uninheritable.alias);
    }
    else
    {
        q = preProcessor->LookupAlias(sym->name);
        if (q)
            return litlate(q);
        else if (sym->sb->compilerDeclared || sym->tp->type == bt_templateparam || sym->tp->type == bt_templateselector)
        {
	    return litlate(sym->name);
        }
        else
        {
            return sym->sb->decoratedName;
        }
    }
}

SimpleSymbol* SymbolManager::Get(struct sym *sym)
{
    if (sym && sym->sb)
    {
        SimpleSymbol *rv;
        rv = Lookup(sym);
        if (!rv)
        {
            rv = Make(sym);
        }
        return rv;
    }
    return nullptr;
}


SimpleSymbol* SymbolManager::Test(struct sym* sym)
{
    if (sym)
    {
        return Lookup(sym);
    }
    return nullptr;
}

SimpleExpression* SymbolManager::Get(struct expr* e)
{
    while (e && (e->type == en_lvalue || e->type == en_not_lvalue || e->type == en_x_string || e->type == en_x_object))
        e = e->left;
    SimpleExpression* rv = (SimpleExpression*)Alloc(sizeof(SimpleExpression));
    rv->sizeFromType = natural_size(e);
    if (e->altdata)
        rv->altData = Get((EXPRESSION*)e->altdata);
    switch (e->type)
    {
    case en_global:
        rv->type = se_global;
        rv->sp = Get(e->v.sp);
        break;
    case en_auto:
        rv->type = se_auto;
        rv->sp = Get(e->v.sp);
        break;
    case en_labcon:
        rv->type = se_labcon;
        rv->i = e->v.i;
        break;
    case en_absolute:
        rv->type = se_absolute;
        rv->sp = Get(e->v.sp);
        break;
    case en_pc:
        rv->type = se_pc;
        rv->sp = Get(e->v.sp);
        break;
    case en_const:
        rv->type = se_const;
        rv->sp = Get(e->v.sp);
        break;
    case en_threadlocal:
        rv->type = se_threadlocal;
        rv->sp = Get(e->v.sp);
        break;
    case en_structelem:
        rv->type = se_structelem;
        rv->sp = Get(e->v.sp);
        break;

    case en_msil_array_init:
        rv->type = se_msil_array_init;
        rv->tp = Get(e->v.tp);
        break;
    case en_msil_array_access:
        rv->type = se_msil_array_access;
        rv->msilArrayTP = Get(e->v.msilArray->tp);
        break;

    case en_func:
        rv->type = se_func;
        rv->ascall = e->v.func->ascall;
        break;
    case en_add:
    case en_arrayadd:
    case en_structadd:
        rv->type = se_add;
        rv->left = Get(e->left);
        rv->right = Get(e->right);
        break;

    case en_sub:
        rv->type = se_sub;
        rv->left = Get(e->left);
        rv->right = Get(e->right);
        break;
    case en_uminus:
        rv->type = se_uminus;
        rv->left = Get(e->left);
        break;
    case en_c_bit:
    case en_c_bool:
    case en_c_c:
    case en_c_s:
    case en_c_i:
    case en_c_l:
    case en_c_ll:
        rv->type = se_i;
        rv->i = reint(e);
        break;
    case en_c_uc:
    case en_c_wc:
    case en_c_u16:
    case en_c_us:
    case en_c_ui:
    case en_c_u32:
    case en_c_ul:
    case en_c_ull:
        rv->type = se_ui;
        rv->i = reint(e);
        break;

    case en_c_f:
    case en_c_d:
    case en_c_ld:
        rv->type = se_f;
        rv->f = refloat(e);
        break;
    case en_c_fi:
    case en_c_di:
    case en_c_ldi:
        rv->type = se_fi;
        rv->f = refloat(e);
        break;
    case en_c_fc:
    case en_c_dc:
    case en_c_ldc:
        rv->type = se_fc;
        rv->c.r = e->v.c->r;
        rv->c.i = e->v.c->i;
        break;
    case en_c_string:
        rv->type = se_string;
        {
            char buf[50000], *dest = buf;
            if (e->string)
            {
                int i;
                bool done = false;
                for (i = 0; i < e->string->size && !done; i++)
                {
                    SLCHAR* s = e->string->pointers[i];
                    int j;
                    for (j = 0; j < s->count; j++)
                    {
                        *dest++ = s->str[j];
                        if (dest - buf >= sizeof(buf) - 1)
                        {
                            done = true;
                            break;
                        }
                    }
                }
            }
            rv->astring.str = (char *)Alloc(dest - buf);
            memcpy(rv->astring.str, buf, dest - buf);
            rv->astring.len = dest - buf;
        }
        break;
    default:
        diag("unknown node in Get(EXPRESSION*)");
        break;
    }
    return rv;
}
SimpleType* SymbolManager::Get(struct typ *tp)
{
    SimpleType* rv = (SimpleType*)Alloc(sizeof(SimpleType));
    bool isConst = isconst(tp);
    bool isVolatile = isvolatile(tp);
    bool isRestrict = isrestrict(tp);
    bool isAtomic = isatomic(tp);
    bool isvalist = false;
    bool istypedef = false;
    while (tp != basetype(tp))
    {
        if (tp->type == bt_va_list)
            isvalist = true;
        if (tp->type == bt_typedef)
            istypedef = true;
        tp = tp->btp;
    }
    if ((isstructured(tp) && basetype(tp)->sp->sb->templateLevel && !basetype(tp)->sp->sb->instantiated) || basetype(tp)->type == bt_auto)
    {
        rv->type = st_i;
        rv->size = getSize(bt_int);
        rv->sizeFromType = -ISZ_UINT;
    }
    else
    {
        rv->type = Get(tp->type);

        if (basetype(tp)->sp)
        {
            rv->sp = Get(basetype(tp)->sp);
        }
        rv->size = tp->size;
        if (!isstructured(tp) && !isfunction(tp) && tp->type != bt_ellipse && basetype(tp)->type != bt_any)
            rv->sizeFromType = sizeFromType(tp);
        else
            rv->sizeFromType = ISZ_ADDR;
        rv->bits = tp->bits;
        rv->startbit = tp->startbit;
        rv->isarray = tp->array;
        rv->isvla = tp->vla;
        rv->va_list = tp->type == bt_va_list;
        if (rv->sp)
        {
            // have to put functions in the type table if unreferenced int the global tables
            // this ignores pointer-to-functions
            if (rv->type == st_func && rv->sp && rv->sp->tp && rv->sp->tp->type == st_func)
                definedFunctions.insert(rv->sp);
            // have to put static member functions in the type table as well...
            if (rv->sp->storage_class == scc_type || rv->sp->storage_class == scc_parameter || (rv->sp->storage_class == scc_cast && rv->sp->tp))
            {
                typeSymbols.push_back(rv->sp);
            }
            else if (rv->sp->storage_class == scc_typedef)
                typedefs.push_back(rv->sp);
        }
        if (tp->type != bt_aggregate && tp->syms && tp->syms->table && rv->sp && !rv->sp->syms)
        {
            SYMLIST *list = tp->syms->table[0];
            LIST **p = &rv->sp->syms;
            while (list)
            {
                *p = (LIST*)Alloc(sizeof(LIST));
                (*p)->data = Get(list->p);
                if (rv->sp->storage_class == scc_type || rv->sp->storage_class == scc_cast)
                {
                    if (list->p->sb->storage_class == sc_static || isfunction(list->p->tp))
                    {
                        SimpleSymbol* ns = (SimpleSymbol*)Alloc(sizeof(SimpleSymbol));
                        *ns = *(SimpleSymbol*)(*p)->data;
                        (*p)->data = ns;
                    }
                    typeSymbols.push_back((SimpleSymbol*)(*p)->data);
                }
                else if (isfunction(tp))
                {
                    typeSymbols.push_back((SimpleSymbol*)(*p)->data);
                }
                if (((SimpleSymbol*)(*p)->data)->storage_class == scc_static || ((SimpleSymbol*)(*p)->data)->storage_class == scc_external)
                    definedFunctions.insert((SimpleSymbol*)(*p)->data);
                p = &(*p)->next;
                list = list->next;
            }


        }
        if (tp->btp)
            rv->btp = Get(tp->btp);
    }
    rv->msil = tp->msil;
    rv->scoped = tp->scoped;
    rv->isconst = isConst;
    rv->isvolatile = isVolatile;
    rv->isrestrict = isRestrict;
    rv->isatomic = isAtomic;
    rv->va_list = isvalist;
    rv->istypedef = istypedef;
    return rv;
}
void refreshBackendParams(SYMBOL* funcsp)
{
    if (isfunction(funcsp->tp))
    {
        basetype(funcsp->tp)->sp = funcsp;
        SYMLIST* hr = basetype(funcsp->tp)->syms->table[0];
        LIST* syms = SymbolManager::Get(funcsp)->syms;
        while (hr && syms)
        {
            SimpleSymbol *sym = (SimpleSymbol*)syms->data;
            if (hr->p->sb->thisPtr && !sym->thisPtr)
            {
                hr = hr->next;
            }
            sym->name = hr->p->name; // update name to prototype in use...
            sym->offset = hr->p->sb->offset;
            SymbolManager::Get(hr->p)->offset = sym->offset;
            hr = hr->next;
            syms = syms->next;
        }
    }
}
SimpleSymbol* SymbolManager::Make(struct sym* sym)
{
    SimpleSymbol* rv = (SimpleSymbol*)Alloc(sizeof(SimpleSymbol));
    rv->name = sym->name;
    rv->align = sym->sb->attribs.inheritable.structAlign ? sym->sb->attribs.inheritable.structAlign
        : getAlign(sc_auto, basetype(sym->tp));
    rv->size = basetype(sym->tp)->size;
    rv->importfile = sym->sb->importfile;
    if (sym->sb->parentNameSpace)
        rv->namespaceName = sym->sb->parentNameSpace->name;
    rv->i = sym->sb->value.i;
    Add(sym, rv);
    rv->storage_class = Get(sym->sb->storage_class);
    if (!isstructured(sym->tp) && !isfunction(sym->tp) && sym->tp->type != bt_ellipse && basetype(sym->tp)->type != bt_any)
        rv->sizeFromType = sizeFromType(sym->tp);
    else
        rv->sizeFromType = ISZ_ADDR;
    rv->offset = sym->sb->offset;
    BaseList **p = &rv->baseClasses;
    BASECLASS *src = sym->sb->baseClasses;
    while (src)
    {
        *p = (BaseList*)Alloc(sizeof(BaseList));
        (*p)->offset = src->offset;
        (*p)->sym = Get(src->cls);
        if ((*p)->sym->tp && (*p)->sym->tp->type == st_i)
            typeSymbols.push_back((*p)->sym);
        src = src->next;
        p = &(*p)->next;
    }
    rv->label = sym->sb->label;
    rv->tp = Get(sym->tp);
    // normalize for middle and backend.   Not modifying the front end data structures because there would be too much ripple...
    if (rv->storage_class == scc_parameter && rv->tp->isarray)
    {
        rv->tp->size = getSize(bt_pointer);
        rv->tp->sizeFromType = ISZ_ADDR;
    }
    rv->parentClass = Get(sym->sb->parentClass);
    rv->msil = sym->sb->msil;
    rv->templateLevel = sym->sb->templateLevel;
    rv->hasParams = sym->sb->paramsize != 0;
    rv->isstructured = isstructured(sym->tp);
    rv->anonymous = sym->sb->anonymous;
    rv->allocate = sym->sb->allocate;
    rv->thisPtr = sym->sb->thisPtr;
    rv->stackblock = sym->sb->stackblock;
    rv->inasm = sym->sb->inasm;
    rv->isimport = sym->sb->attribs.inheritable.linkage2 == lk_import;
    rv->isexport = sym->sb->attribs.inheritable.linkage2 == lk_export;
    rv->isvirtual = sym->sb->attribs.inheritable.linkage == lk_virtual;
    rv->msil_rtl = sym->sb->attribs.inheritable.linkage2 == lk_msil_rtl;
    rv->isproperty = sym->sb->attribs.inheritable.linkage2 == lk_property;
    rv->unmanaged = sym->sb->attribs.inheritable.linkage2 == lk_unmanaged;
    rv->isstdcall = sym->sb->attribs.inheritable.linkage == lk_stdcall;
    rv->iscdecl = sym->sb->attribs.inheritable.linkage == lk_cdecl;
    rv->ispascal = sym->sb->attribs.inheritable.linkage == lk_pascal;
    rv->isfastcall = sym->sb->attribs.inheritable.linkage == lk_fastcall;
    rv->entrypoint = sym->sb->attribs.inheritable.linkage3 == lk_entrypoint;
    rv->ispure = sym->sb->ispure;
    rv->dontinstantiate = sym->sb->dontinstantiate;
    rv->noextern = sym->sb->noextern;
    rv->initialized = !!sym->sb->init;
    rv->importThunk = sym->sb->importThunk;

    rv->inFunc = theCurrentFunc != nullptr;
    rv->temp = sym->sb->temp;
    rv->vbase = !!sym->sb->vbaseEntries;
    rv->anyTry = sym->sb->anyTry;
    rv->xc = !!sym->sb->xc;
    rv->canThrow = sym->sb->canThrow;
    rv->hasInlineFunc = sym->sb->inlineFunc.stmt != nullptr;
    rv->usesEsp = cparams.prm_useesp;

    rv->outputName = beDecorateSymName(sym);

    return rv;
}
st_type SymbolManager::Get(enum e_bt type)
{

    switch (type)
    {
    case bt_char:
    case bt_bool:
    case bt_short:
    case bt_int:
    case bt_long:
    case bt_long_long:
    case bt_inative:
        return st_i;
    case bt_unsigned_char:
    case bt_unsigned_short:
    case bt_char16_t:
    case bt_wchar_t:
    case bt_unsigned:
    case bt_unsigned_long:
    case bt_unsigned_long_long:
    case bt_char32_t:
    case bt_unative:
        return st_ui;
    case bt_float:
    case bt_double:
    case bt_long_double:
        return st_f;
    case bt_float_imaginary:
    case bt_double_imaginary:
    case bt_long_double_imaginary:
        return st_fi;
    case bt_float_complex:
    case bt_double_complex:
    case bt_long_double_complex:
        return st_fc;
    case bt_pointer:
        return st_pointer;
    case bt_void:
        return st_void;
    case bt___string:
        return st___string;
    case bt___object:
        return st___object;
    case bt_func:
    case bt_ifunc:
        return st_func;
    case bt_lref:
        return st_lref;
    case bt_rref:
        return st_rref;
    case bt_struct:
    case bt_class:
        return st_struct;
    case bt_union:
        return st_union;
    case bt_enum:
        return st_enum;
    case bt_memberptr:
        return st_memberptr;
    case bt_aggregate:
        return st_aggregate;
    case bt_ellipse:
        return st_ellipse;
    case bt_any:
    default:
        return st_any;
    }

}

e_scc_type SymbolManager::Get(enum e_sc storageClass)
{
    switch (storageClass)
    {
    default:
    case sc_none:
        return scc_none;
    case sc_static:
        return scc_static;
    case sc_localstatic:
        return scc_localstatic;
    case sc_auto:
        return scc_auto;
    case sc_register:
        return scc_register;
    case sc_global:
        return scc_global;
    case sc_external:
        return scc_external;
    case sc_templateparam:
        return scc_templateparam;
    case sc_parameter:
        return scc_parameter;
    case sc_catchvar:
        return scc_catchvar;
    case sc_type:
        return scc_type;
    case sc_typedef:
        return scc_typedef;
    case sc_member:
        return scc_member;
    case sc_mutable:
        return scc_mutable;
    case sc_cast:
        return scc_cast;
    case sc_defunc:
        return scc_defunc;
    case sc_label:
        return scc_label;
    case sc_ulabel:
        return scc_ulabel;
    case sc_overloads:
        return scc_overloads;
    case sc_constant:
        return scc_constant;
    case sc_enumconstant:
        return scc_enumconstant;
    case sc_absolute:
        return scc_absolute;
    case sc_friendlist:
        return scc_friendlist;
    case sc_const:
        return scc_const;
    case sc_tconst:
        return scc_tconst;
    case sc_classmember:
        return scc_classmember;
    case sc_constexpr:
        return scc_constexpr;
    case sc_memberreg:
        return scc_memberreg;
    case sc_namespace:
        return scc_namespace;
    case sc_namespacealias:
        return scc_namespacealias;
    case sc_temp:
        return scc_temp;
    case sc_virtual:
        return scc_virtual;
    }
}
unsigned long long SymbolManager::Key(struct sym* old)
{
    char buf[8192];
    buf[0] = 0;
    if (old->sb->parent)
    {
        strcat(buf, old->sb->parent->sb->decoratedName);
        my_sprintf(buf + strlen(buf), "%d", old->sb->uniqueID);
    }
    strcat(buf, old->sb->decoratedName ? old->sb->decoratedName : old->name);
    if (old->sb->storage_class == sc_type)
        strcat(buf, "#");
    if (old->sb->attribs.inheritable.linkage == lk_stdcall)
        strcat(buf, "$");
    std::hash<std::string> hasher;
    std::string aa(buf);
    size_t key = hasher(aa);
    std::reverse(aa.begin(), aa.end());
    size_t key2 = hasher(aa);
    return ((unsigned long long)key << 32) | key2;
}
SimpleSymbol* SymbolManager::Lookup(struct sym* old)
{
    if (old->sb->symRef)
        return old->sb->symRef;
    SimpleSymbol *rv = symbols[Key(old)];
    if (rv)
        old->sb->symRef = rv;
    return rv;
}
void SymbolManager::Add(struct sym* old, SimpleSymbol* sym)
{
    old->sb->symRef = sym;
    symbols[Key(old)] = sym;
    switch (sym->storage_class)
    {
    case scc_member:
        if (sym->tp->type != st_func)
            break;
        // fallthrough
    case scc_global:
    case scc_static:
    case scc_localstatic:
    case scc_external:
    case scc_virtual:
        globalSymbols[sym->outputName] = sym;
        break;
    }

}