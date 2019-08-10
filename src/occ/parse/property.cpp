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

#include "compiler.h"

extern ARCH_ASM* chosenAssembler;
extern enum e_kw skim_end[];
extern NAMESPACEVALUELIST* globalNameSpace;
extern TYPE stdvoid;
extern int total_errors;
extern int startlab;
extern int retlab;
extern int nextLabel;

static SYMBOL* CreateSetterPrototype(SYMBOL* sym)
{
    SYMBOL*rv, *value;
    char name[512];
    sprintf(name, "set_%s", sym->name);
    rv = makeID(sym->storage_class, nullptr, nullptr, litlate(name));
    value = makeID(sc_parameter, sym->tp, nullptr, "value");
    value->attribs.inheritable.used = true;  // to avoid unused variable errors
    rv->access = ac_public;
    rv->tp = (TYPE*)(TYPE *)Alloc(sizeof(TYPE));
    rv->tp->sp = rv;
    rv->tp->type = bt_func;
    rv->tp->btp = &stdvoid;
    rv->tp->syms = CreateHashTable(1);
    SetLinkerNames(value, lk_cdecl);
    insert(value, rv->tp->syms);
    SetLinkerNames(rv, lk_cdecl);
    return rv;
}
static SYMBOL* CreateGetterPrototype(SYMBOL* sym)
{
    SYMBOL*rv, *nullparam;
    char name[512];
    sprintf(name, "get_%s", sym->name);
    rv = makeID(sym->storage_class, nullptr, nullptr, litlate(name));
    nullparam = makeID(sc_parameter, &stdvoid, nullptr, "__void");
    rv->access = ac_public;
    rv->tp = (TYPE*)(TYPE *)Alloc(sizeof(TYPE));
    rv->tp->sp = rv;
    rv->tp->type = bt_func;
    rv->tp->btp = sym->tp;
    rv->tp->syms = CreateHashTable(1);
    SetLinkerNames(nullparam, lk_cdecl);
    insert(nullparam, rv->tp->syms);
    SetLinkerNames(rv, lk_cdecl);
    return rv;
}
static void insertfunc(SYMBOL* in, HASHTABLE* syms)
{

    SYMLIST** hr = LookupName(in->name, syms);
    SYMBOL* funcs = nullptr;
    if (hr)
        funcs = (SYMBOL*)((*hr)->p);
    if (!funcs)
    {
        TYPE* tp = (TYPE*)(TYPE *)Alloc(sizeof(TYPE));
        tp->type = bt_aggregate;
        tp->rootType = tp;
        funcs = makeID(sc_overloads, tp, 0, litlate(in->name));
        tp->sp = funcs;
        SetLinkerNames(funcs, lk_cdecl);
        insert(funcs, syms);
        funcs->tp->syms = CreateHashTable(1);
        insert(in, funcs->tp->syms);
        in->overloadName = funcs;
    }
    else if (funcs->storage_class == sc_overloads)
    {
        insertOverload(in, funcs->tp->syms);
        in->overloadName = funcs;
    }
    else
    {
        fatal("insertfuncs: invalid overload tab");
    }
}
static SYMBOL* CreateBackingVariable(SYMBOL* sym)
{
    char name[512];
    SYMBOL* rv;
    sprintf(name, "__backing_%s", sym->name);
    rv = makeID(sc_static, sym->tp, nullptr, litlate(name));
    rv->label = nextLabel++;
    SetLinkerNames(rv, lk_cdecl);
    return rv;
}
static SYMBOL* CreateBackingSetter(SYMBOL* sym, SYMBOL* backing)
{
    SYMBOL* p = CreateSetterPrototype(sym);
    STATEMENT* st;
    BLOCKDATA b;
    EXPRESSION* left = varNode(en_global, backing);
    EXPRESSION* right = varNode(en_global, (SYMBOL*)p->tp->syms->table[0]->p);
    p->tp->type = bt_ifunc;
    memset(&b, 0, sizeof(b));
    deref(sym->tp, &left);
    deref(sym->tp, &right);
    st = stmtNode(nullptr, &b, st_expr);
    st->select = exprNode(en_assign, left, right);
    p->inlineFunc.stmt = stmtNode(nullptr, nullptr, st_block);
    p->inlineFunc.stmt->lower = b.head;
    p->inlineFunc.syms = p->tp->syms;
    return p;
}
static SYMBOL* CreateBackingGetter(SYMBOL* sym, SYMBOL* backing)
{
    SYMBOL* p = CreateGetterPrototype(sym);
    STATEMENT* st;
    BLOCKDATA b;
    p->tp->type = bt_ifunc;
    memset(&b, 0, sizeof(b));
    st = stmtNode(nullptr, &b, st_return);
    st->select = varNode(en_global, backing);
    deref(sym->tp, &st->select);
    p->inlineFunc.stmt = stmtNode(nullptr, nullptr, st_block);
    p->inlineFunc.stmt->lower = b.head;
    p->inlineFunc.syms = p->tp->syms;
    return p;
}
LEXEME* initialize_property(LEXEME* lex, SYMBOL* funcsp, SYMBOL* sym, enum e_sc storage_class_in, bool asExpression, int flags)
{
    if (isstructured(sym->tp))
        error(ERR_ONLY_SIMPLE_PROPERTIES_SUPPORTED);
    if (funcsp || sym->storage_class == sc_parameter)
        error(ERR_NO_PROPERTY_IN_FUNCTION);
    if (sym->storage_class != sc_external)
    {
        if (MATCHKW(lex, begin))
        {
            SYMBOL*get = nullptr, *set = nullptr;
            SYMBOL* prototype;
            lex = getsym();
            while (ISID(lex))
            {
                bool err = false;
                if (!strcmp(lex->value.s.a, "get"))
                {
                    if (get)
                    {
                        errorsym(ERR_GETTER_ALREADY_EXISTS, sym);
                        err = true;
                    }
                    else
                    {
                        get = prototype = CreateGetterPrototype(sym);
                    }
                }
                else if (!strcmp(lex->value.s.a, "set"))
                {
                    if (set)
                    {
                        errorsym(ERR_SETTER_ALREADY_EXISTS, sym);
                        err = true;
                    }
                    else
                    {
                        set = prototype = CreateSetterPrototype(sym);
                    }
                }
                else
                {
                    error(ERR_ONLY_SIMPLE_PROPERTIES_SUPPORTED);
                    err = true;
                }
                lex = getsym();
                if (err)
                {
                    needkw(&lex, begin);
                    errskim(&lex, skim_end);
                    needkw(&lex, end);
                }
                else
                {
                    int old = GetGlobalFlag();
                    SetGlobalFlag(old + 1);
                    lex = body(lex, prototype);
                    SetGlobalFlag(old);
                    insertfunc(prototype, globalNameSpace->valueData->syms);
                }
            }
            if (!get)
                errorsym(ERR_MUST_DECLARE_PROPERTY_GETTER, sym);
            if (set)
                sym->has_property_setter = true;
            chosenAssembler->msil->create_property(sym, get, set);
            needkw(&lex, end);
        }
        else  // create default getter and setter
        {
            SYMBOL* backing = CreateBackingVariable(sym);
            SYMBOL* setter = CreateBackingSetter(sym, backing);
            SYMBOL* getter = CreateBackingGetter(sym, backing);
            GENREF(backing);
            GENREF(setter);
            GENREF(getter);
            insertfunc(setter, globalNameSpace->valueData->syms);
            insertfunc(getter, globalNameSpace->valueData->syms);
            insert(backing, globalNameSpace->valueData->syms);
            if (!total_errors)
            {
                int oldstartlab = startlab;
                int oldretlab = retlab;
                startlab = nextLabel++;
                retlab = nextLabel++;
                genfunc(getter, true);
                retlab = oldretlab;
                startlab = oldstartlab;
            }
            if (!total_errors)
            {
                int oldstartlab = startlab;
                int oldretlab = retlab;
                startlab = nextLabel++;
                retlab = nextLabel++;
                genfunc(setter, true);
                retlab = oldretlab;
                startlab = oldstartlab;
            }
            insertInitSym(backing);
            chosenAssembler->msil->create_property(sym, getter, setter);
            sym->has_property_setter = true;
        }
    }
    return lex;
}
