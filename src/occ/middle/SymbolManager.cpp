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

std::unordered_map<unsigned, SimpleSymbol*> SymbolManager::symbols;

SimpleSymbol* SymbolManager::Get(struct sym *sym)
{
    SimpleSymbol *rv = symbols[sym->key];
    if (!rv)
    {
        rv = Make(sym);
        symbols[sym->key] = rv;
    }
    return rv;
}

SimpleExpression* SymbolManager::Get(struct expr* e)
{
    while (e && e->type == en_lvalue || e->type == en_not_lvalue)
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
        rv->string = e->string;
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
    return rv;
}
SimpleSymbol* SymbolManager::Make(struct sym* sym)
{
    SimpleSymbol* rv = (SimpleSymbol*)Alloc(sizeof(SimpleSymbol));
    rv->decoratedName = sym->decoratedName;
    rv->align = sym->attribs.inheritable.structAlign ? sym->attribs.inheritable.structAlign
        : getAlign(sc_auto, basetype(sym->tp));
    rv->size = basetype(sym->tp)->size;
    EXPRESSION *node = (EXPRESSION*)sym->inlineFunc.stmt;
    if (node)
        rv->paramSubstitute = SymbolManager::Get(node->left->v.sp);

    return rv;
}
bool comparetypes(SimpleType* typ1, SimpleType* typ2, int exact)
{
    if (typ1->type == st_any || typ2->type == st_any)
        return true;
    while (typ1->type == st_typedef)
        typ1 = typ1->btp;
    while (typ2->type == st_typedef)
        typ2 = typ2->btp;
    while (typ1->type == st_rref || typ1->type == st_lref)
        typ1 = typ1->btp;
    while (typ2->type == st_rref || typ2->type == st_lref)
        typ2 = typ2->btp;
    while (typ1->type == st_typedef)
        typ1 = typ1->btp;
    while (typ2->type == st_typedef)
        typ2 = typ2->btp;
    if (typ1->type == st_pointer && typ2->type == st_pointer)
    {
        if (exact)
        {
            int arr = false;
            while (typ1->type == st_pointer && typ2->type == st_pointer)
            {
                if (typ1->type != typ2->type)
                    return false;
                if (typ1->msil != typ2->msil)
                    return false;
                if (arr && (typ1->isarray != typ2->isarray))
                    return false;
                if (arr && typ1->size != typ2->size)
                    return false;
                arr |= typ1->isarray | typ2->isarray;
                typ1 = typ1->btp;
                typ2 = typ2->btp;
            }
            return comparetypes(typ1, typ2, true);
        }

        else
            return true;
    }
    if (exact && (typ1->type == st_func || (typ1->type == st_pointer && typ1->btp->type == st_func)) && 
        (typ2->type == st_func || (typ2->type == st_pointer && typ2->btp->type == st_func)))
    {

        if (typ1->type == st_pointer)
            typ1 = typ1->btp;
        if (typ2->type == st_pointer)
            typ2 = typ2->btp;
        if (!comparetypes(typ1->btp, typ2->btp, exact))
            return false;
        LIST *l1 = typ1->syms;
        LIST *l2 = typ2->syms;
        while (l1 && l2)
        {
            SimpleSymbol *s1 = (SimpleSymbol*)l1->data;
            SimpleSymbol *s2 = (SimpleSymbol*)l2->data;
            if (!comparetypes(s1->tp, s2->tp, exact))
                return false;
        }
        return !l1 && !l2;
    }
    if (cparams.prm_cplusplus)
    {
        if (typ1->scoped != typ2->scoped)
            return false;
        if (typ1->type == st_enum)
        {
            if (typ2->type == st_enum)
                return typ1->sp == typ2->sp;
            else
                return (typ2->type == st_i || typ2->type == st_ui);
        }
        else if (typ2->type == st_enum)
        {
            return (typ1->type == st_i || typ1->type == st_ui);
        }
        if (typ1->type == typ2->type && typ1->type == st_memberptr)
        {
            if (typ1->sp != typ2->sp)
            {
//                if (classRefCount(typ1->sp, typ2->sp) != 1)
                    return false;
            }
            return comparetypes(typ1->btp, typ2->btp, exact);
        }
    }
    if (typ1->type == typ2->type && typ1->type == st___string)
        return true;
    if (typ1->type == st___object)  // object matches anything
        return true;
    if (typ1->type == typ2->type && (typ1->type == st_struct || typ1->type == st_union || (exact && typ1->type == st_enum)))
        return typ1->sp == typ2->sp;
    if (typ1->type == typ2->type || (!exact && typ2->type < st_void && typ1->type < st_void))
        return true;
    if (typ1->type == st_func && typ2->type == st_func)
        return true;
    else if (!exact && ((typ1->type == st_pointer && ((typ2->type == st_pointer && typ2->btp->type == st_func) || typ2->type == st_func || typ2->type == st_i || typ2->type == st_ui)) ||
        (typ2->type == st_pointer && ((typ1->type == st_pointer && typ1->btp->type == st_func) || typ1->type == st_func || typ1->type == st_i || typ1->type == st_ui))))
        return (true);
    else if (typ1->type == st_enum && (typ2->type == st_i || typ2->type == st_ui))
    {
        return true;
    }
    else if (typ2->type == st_enum && (typ1->type == st_i || typ1->type == st_ui))
    {
        return true;
    }
    return false;
}

bool equalnode(SimpleExpression* left, SimpleExpression *right)
{
    if (left->type != right->type)
        return false;
    switch (left->type)
    {
    case en_msil_array_init:
        return comparetypes(left->tp, right->tp, true);
    case se_add:
        return equalnode(left->left, right->left) && equalnode(left->right, right->right); // this is naive but...
    case se_i:
    case se_labcon:
        return left->i == right->i;
    case se_f:
    case se_fi:
        return left->f == right->f;
    case se_fc:
        return left->c.i == right->c.i && left->c.r == right->c.r;
    default:
        return left->sp->key == right->sp->key;

    }
}
SimpleExpression* GetSymRef(SimpleExpression* n)
{
    SimpleExpression* rv = nullptr;
    std::stack<SimpleExpression*> st;
    st.push(n);
    while (!st.empty())
    {
        SimpleExpression* exp = st.top();
        st.pop();
        switch (exp->type)
        {
        case se_labcon:
        case se_global:
        case se_auto:
        case se_absolute:
        case se_pc:
        case se_threadlocal:
            return exp;
        case se_tempref:
            if (st.empty())
                return nullptr;
            continue;
        default:
            if (!isintconst(exp) && !isfloatconst(exp))
            {
                if (exp->right)
                {
                    st.push(exp->right);
                }
                if (exp->left)
                {
                    st.push(exp->left);
                }
            }
            break;
        }
    }
    return rv;
}
