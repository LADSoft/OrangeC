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
#include "iexpr.h"
#include <stack>
#include "beinterf.h"



std::unordered_map<std::string, SimpleSymbol*> SymbolManager::symbols;
std::unordered_map<std::string, SimpleSymbol*> SymbolManager::globalSymbols;

extern COMPILER_PARAMS cparams;

bool comparetypes(SimpleType* typ1, SimpleType* typ2, int exact)
{
    if (typ1->type == st_any || typ2->type == st_any)
        return true;
    while (typ1->type == st_rref || typ1->type == st_lref)
        typ1 = typ1->btp;
    while (typ2->type == st_rref || typ2->type == st_lref)
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
        if (!!typ1->sp != !!typ2->sp)
            return false;
        if (typ1->sp)
        {
            LIST *l1 = typ1->sp->syms;
            LIST *l2 = typ2->sp->syms;
            while (l1 && l2)
            {
                SimpleSymbol *s1 = (SimpleSymbol*)l1->data;
                SimpleSymbol *s2 = (SimpleSymbol*)l2->data;
                if (!comparetypes(s1->tp, s2->tp, exact))
                    return false;
            }

            return !l1 && !l2;
        }
        return true;
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
    case se_msil_array_init:
        return comparetypes(left->tp, right->tp, true);
    case se_uminus:
        return equalnode(left->left, right->left);
    case se_add:
    case se_sub:
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
        return left->sp == right->sp;

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
            if (exp->type != se_i && exp->type != se_ui && exp->type != se_f && exp->type != se_fi && exp->type != se_fc)
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
SimpleSymbol* SymbolManager::Get(const char* name)
{
    auto it = globalSymbols.find(name);
    if (it == globalSymbols.end())
        return nullptr;
    return it->second;
}
