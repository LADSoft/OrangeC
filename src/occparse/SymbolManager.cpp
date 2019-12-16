/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
#include "PreProcessor.h"
extern PreProcessor* preProcessor;
extern SYMBOL* theCurrentFunc;
extern std::vector<SimpleSymbol*> typeSymbols;
extern std::vector<SimpleSymbol*> typedefs;

void SymbolManager::clear()
{
    if (architecture != ARCHITECTURE_MSIL || (cparams.prm_compileonly && !cparams.prm_asmfile))
        symbols.clear(); globalSymbols.clear();
}

void beDecorateSymName(char* buf, SYMBOL* sym)
{
    const char* q;
    if (sym->attribs.uninheritable.alias)
    {
        strcpy(buf, sym->attribs.uninheritable.alias);
    }
    else
    {
        q = preProcessor->LookupAlias(sym->name);
        if (q)
            strcpy(buf, q);
        else if (sym->compilerDeclared || sym->tp->type == bt_templateparam || sym->tp->type == bt_templateselector)
        {
            strcpy(buf, sym->name);
        }
        else
        {
            strcpy(buf, sym->decoratedName);
        }
    }
}

SimpleSymbol* SymbolManager::Get(struct sym *sym)
{
    if (sym)
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

SimpleExpression* SymbolManager::Get(struct expr* e)
{
    while (e && e->type == en_lvalue || e->type == en_not_lvalue || e->type == en_x_string || e->type == en_x_object)
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
        rv->c.r = e->v.c.r;
        rv->c.i = e->v.c.i;
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
    while (tp != basetype(tp))
    {
        if (tp->type == bt_va_list)
            isvalist = true;
        tp = tp->btp;
    }
    if ((isstructured(tp) && basetype(tp)->sp->templateLevel && !basetype(tp)->sp->instantiated) || basetype(tp)->type == bt_auto)
    {
        rv->type = st_i;
        rv->size = getSize(bt_int);
        rv->sizeFromType = -ISZ_UINT;
    }
    else
    {
        rv->type = Get(tp->type);
        if (tp->sp)
        {
            rv->sp = Get(tp->sp);
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
            if (rv->sp->storage_class == scc_type || rv->sp->storage_class == scc_parameter || rv->sp->storage_class == scc_cast)
                typeSymbols.push_back(rv->sp);
            else if (rv->sp->storage_class == scc_typedef)
                typedefs.push_back(rv->sp);
        }
        if (tp->syms && tp->syms->table && rv->sp && !rv->sp->syms)
        {
            SYMLIST *list = tp->syms->table[0];
            LIST **p = &rv->sp->syms;
            SYMLIST *old = nullptr;
            while (list)
            {
                if (list->p->tp->type == bt_aggregate)
                {
                    old = list->next;
                    list = list->p->tp->syms->table[0];
                }
                *p = (LIST*)Alloc(sizeof(LIST));
                (*p)->data = Get(list->p);
                if (rv->sp->storage_class == scc_type || rv->sp->storage_class == scc_cast)
                {
                    if (list->p->storage_class == sc_static || isfunction(list->p->tp))
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
                p = &(*p)->next;
                list = list->next;
                if (!list)
                {
                    list = old;
                    old = nullptr;
                }
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
    return rv;
}
void refreshBackendParams(SYMBOL* funcsp)
{
    if (isfunction(funcsp->tp))
    {
        SYMLIST* hr = basetype(funcsp->tp)->syms->table[0];
        LIST* syms = SymbolManager::Get(funcsp)->syms;
        while (hr && syms)
        {
            SimpleSymbol *sym = (SimpleSymbol*)syms->data;
            if (hr->p->thisPtr && !sym->thisPtr)
            {
                hr = hr->next;
            }
            sym->name = hr->p->name; // update name to prototype in use...
            sym->offset = hr->p->offset;
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
    rv->align = sym->attribs.inheritable.structAlign ? sym->attribs.inheritable.structAlign
        : getAlign(sc_auto, basetype(sym->tp));
    rv->size = basetype(sym->tp)->size;
    rv->importfile = sym->importfile;
    if (sym->parentNameSpace)
        rv->namespaceName = sym->parentNameSpace->name;
    rv->i = sym->value.i;
    Add(sym, rv);
    rv->storage_class = Get(sym->storage_class);
    if (!isstructured(sym->tp) && !isfunction(sym->tp) && sym->tp->type != bt_ellipse && basetype(sym->tp)->type != bt_any)
        rv->sizeFromType = sizeFromType(sym->tp);
    else
        rv->sizeFromType = ISZ_ADDR;
    rv->offset = sym->offset;
    BaseList **p = &rv->baseClasses;
    BASECLASS *src = sym->baseClasses;
    while (src)
    {
        *p = (BaseList*)Alloc(sizeof(BaseList));
        (*p)->offset = src->offset;
        (*p)->sym = Get(src->cls);
        if ((*p)->sym->tp->type == st_i)
            typeSymbols.push_back((*p)->sym);
        src = src->next;
        p = &(*p)->next;
    }
    rv->label = sym->label;
    rv->tp = Get(sym->tp);
    rv->parentClass = Get(sym->parentClass);
    rv->msil = sym->msil;
    rv->templateLevel = sym->templateLevel;
    rv->hasParams = sym->paramsize != 0;
    rv->isstructured = isstructured(sym->tp);
    rv->anonymous = sym->anonymous;
    rv->allocate = sym->allocate;
    rv->thisPtr = sym->thisPtr;
    rv->stackblock = sym->stackblock;
    rv->inasm = sym->inasm;
    rv->isimport = sym->attribs.inheritable.linkage2 == lk_import;
    rv->isexport = sym->attribs.inheritable.linkage2 == lk_export;
    rv->isvirtual = sym->attribs.inheritable.linkage == lk_virtual;
    rv->msil_rtl = sym->attribs.inheritable.linkage2 == lk_msil_rtl;
    rv->isproperty = sym->attribs.inheritable.linkage2 == lk_property;
    rv->unmanaged = sym->attribs.inheritable.linkage2 == lk_unmanaged;
    rv->isstdcall = sym->attribs.inheritable.linkage == lk_stdcall;
    rv->iscdecl = sym->attribs.inheritable.linkage == lk_cdecl;
    rv->ispascal = sym->attribs.inheritable.linkage == lk_pascal;
    rv->isfastcall = sym->attribs.inheritable.linkage == lk_fastcall;
    rv->entrypoint = sym->attribs.inheritable.linkage3 == lk_entrypoint;
    rv->ispure = sym->ispure;
    rv->dontinstantiate = sym->dontinstantiate;
    rv->noextern = sym->noextern;
    rv->initialized = !!sym->init;

    rv->inFunc = theCurrentFunc != nullptr;
    rv->temp = sym->temp;
    rv->vbase = !!sym->vbaseEntries;
    rv->anyTry = sym->anyTry;
    rv->xc = !!sym->xc;
    rv->canThrow = sym->canThrow;
    rv->hasInlineFunc = sym->inlineFunc.stmt != nullptr;
    rv->usesEsp = cparams.prm_useesp;

    char buf[8192];
    beDecorateSymName(buf, sym);
    rv->outputName = litlate(buf);

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
    case bt_typedef:
        return st_typedef;
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
const char* SymbolManager::Key(struct sym* old)
{
    if (!old->key)
    {
        char buf[8192];
        buf[0] = 0;
        if (old->parent)
        {
            strcat(buf, old->parent->decoratedName);
            sprintf(buf + strlen(buf), "%d", old->uniqueID);
        }
        strcat(buf, old->decoratedName ? old->decoratedName : old->name);
        if (old->storage_class == sc_type)
            strcat(buf, "#");
        old->key = litlate(buf);
    }
    return old->key;
}
SimpleSymbol* SymbolManager::Lookup(struct sym* old)
{
    return symbols[Key(old)];
}
void SymbolManager::Add(struct sym* old, SimpleSymbol* sym)
{
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