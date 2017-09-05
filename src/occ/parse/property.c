/*
Software License Agreement (BSD License)

Copyright (c) 1997-2016, David Lindauer, (LADSoft).
All rights resesped.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the following
conditions are met:

* Redistributions of source code must retain the above
copyright notice, this list of conditions and the
following disclaimer.

* Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the
following disclaimer in the documentation and/or other
materials provided with the distribution.

* Neither the name of LADSoft nor the names of its
contributors may be used to endorse or promote products
derived from this software without specific prior
written permission of LADSoft.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SEspICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include "compiler.h"

extern ARCH_ASM *chosenAssembler;
extern enum e_kw skim_end[];
extern NAMESPACEVALUES *globalNameSpace;
extern TYPE stdvoid;
extern int total_errors;
extern int startlab;
extern int retlab;
extern int nextLabel;

static SYMBOL *CreateSetterPrototype(SYMBOL *sp)
{
    SYMBOL *rv, *value;
    char name[512];
    sprintf(name, "set_%s", sp->name);
    rv = makeID(sp->storage_class, NULL, NULL, litlate(name));
    value = makeID(sc_parameter, sp->tp, NULL, "value");
    value->used = TRUE; // to avoid unused variable errors
    rv->access = ac_public;
    rv->tp = (TYPE *)Alloc(sizeof(TYPE));
    rv->tp->sp = rv;
    rv->tp->type = bt_func;
    rv->tp->btp = &stdvoid;
    rv->tp->syms = CreateHashTable(1);
    SetLinkerNames(value, lk_cdecl);
    insert(value, rv->tp->syms);
    SetLinkerNames(rv, lk_cdecl);
    return rv;
}
static SYMBOL *CreateGetterPrototype(SYMBOL *sp)
{
    SYMBOL *rv, *nullparam;
    char name[512];
    sprintf(name, "get_%s", sp->name);
    rv = makeID(sp->storage_class, NULL, NULL, litlate(name));
    nullparam= makeID(sc_parameter, &stdvoid, NULL, "__void");
    rv->access = ac_public;
    rv->tp = (TYPE *)Alloc(sizeof(TYPE));
    rv->tp->sp = rv;
    rv->tp->type = bt_func;
    rv->tp->btp = sp->tp;
    rv->tp->syms = CreateHashTable(1);
    SetLinkerNames(nullparam, lk_cdecl);
    insert(nullparam, rv->tp->syms);
    SetLinkerNames(rv, lk_cdecl);
    return rv;

}
static void insertfunc(SYMBOL *in, HASHTABLE *syms)
{

    HASHREC **hr = LookupName(in->name, syms);
    SYMBOL *funcs = NULL;
    if (hr)
        funcs = (SYMBOL *)((*hr)->p);
    if (!funcs)
    {
        TYPE *tp = (TYPE *)Alloc(sizeof(TYPE));
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
static SYMBOL *CreateBackingVariable(SYMBOL *sp)
{
    char name[512];
    SYMBOL *rv;
    sprintf(name, "__backing_%s", sp->name);
    rv = makeID(sc_static, sp->tp, NULL, litlate(name));
    rv->label = nextLabel++;
    SetLinkerNames(rv, lk_cdecl);
    return rv;
}
static SYMBOL *CreateBackingSetter(SYMBOL *sp, SYMBOL *backing)
{
    SYMBOL *p = CreateSetterPrototype(sp);
    STATEMENT *st;
    BLOCKDATA b;
    EXPRESSION *left = varNode(en_global, backing);
    EXPRESSION *right = varNode(en_global, (SYMBOL *)p->tp->syms->table[0]->p);
    p->tp->type = bt_ifunc;
    memset(&b, 0, sizeof(b));
    deref(sp->tp, &left);
    deref(sp->tp, &right);
    st = stmtNode(NULL, &b, st_expr);
    st->select = exprNode(en_assign, left, right);
    p->inlineFunc.stmt = stmtNode(NULL, NULL, st_block);
    p->inlineFunc.stmt->lower = b.head;
    p->inlineFunc.syms = p->tp->syms;
    return p;
}
static SYMBOL *CreateBackingGetter(SYMBOL *sp, SYMBOL *backing)
{
    SYMBOL *p = CreateGetterPrototype(sp);
    STATEMENT *st;
    BLOCKDATA b;
    p->tp->type = bt_ifunc;
    memset(&b, 0, sizeof(b));
    st = stmtNode(NULL, &b, st_return);
    st->select = varNode(en_global, backing);
    deref(sp->tp, &st->select);
    p->inlineFunc.stmt = stmtNode(NULL, NULL, st_block);
    p->inlineFunc.stmt->lower = b.head;
    p->inlineFunc.syms = p->tp->syms;
    return p;
}
LEXEME *initialize_property(LEXEME *lex, SYMBOL *funcsp, SYMBOL *sp, enum e_sc storage_class_in, BOOLEAN asExpression, int flags)
{
    if (isstructured(sp->tp))
        error(ERR_ONLY_SIMPLE_PROPERTIES_SUPPORTED);
    if (funcsp || sp->storage_class == sc_parameter)
        error(ERR_NO_PROPERTY_IN_FUNCTION);
    if (sp->storage_class != sc_external)
    {
        if (MATCHKW(lex, begin))
        {
            SYMBOL *get=NULL, *set = NULL;
            SYMBOL *prototype;
            lex = getsym();
            while (ISID(lex))
            {
                BOOLEAN err = FALSE;
                if (!strcmp(lex->value.s.a, "get"))
                {
                    if (get)
                    {
                        errorsym(ERR_GETTER_ALREADY_EXISTS, sp);
                        err = TRUE;
                    }
                    else
                    {
                        get = prototype = CreateGetterPrototype(sp);
                    }
                }
                else if (!strcmp(lex->value.s.a, "set"))
                {
                    if (set)
                    {
                        errorsym(ERR_SETTER_ALREADY_EXISTS, sp);
                        err = TRUE;
                    }
                    else
                    {
                        set = prototype = CreateSetterPrototype(sp);
                    }
                }
                else
                {
                    error(ERR_ONLY_SIMPLE_PROPERTIES_SUPPORTED);
                    err = TRUE;
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
                    insertfunc(prototype, globalNameSpace->syms);
                }
            }
            if (!get)
                errorsym(ERR_MUST_DECLARE_PROPERTY_GETTER, sp);
            if (set)
                sp->has_property_setter = TRUE;
            chosenAssembler->msil->create_property(sp, get, set);
            needkw(&lex, end);
        }
        else // create default getter and setter
        {
            SYMBOL *backing = CreateBackingVariable(sp);
            SYMBOL *setter = CreateBackingSetter(sp, backing);
            SYMBOL *getter = CreateBackingGetter(sp, backing);
            GENREF(backing);
            GENREF(setter);
            GENREF(getter);
            insertfunc(setter, globalNameSpace->syms);
            insertfunc(getter, globalNameSpace->syms);
            insert(backing, globalNameSpace->syms);
            if (!total_errors)
            {
                int oldstartlab = startlab;
                int oldretlab = retlab;
                startlab = nextLabel++;
                retlab = nextLabel++;
                genfunc(getter, TRUE);
                retlab = oldretlab;
                startlab = oldstartlab;
            }
            if (!total_errors)
            {
                int oldstartlab = startlab;
                int oldretlab = retlab;
                startlab = nextLabel++;
                retlab = nextLabel++;
                genfunc(setter, TRUE);
                retlab = oldretlab;
                startlab = oldstartlab;
            }
            insertInitSym(backing);
            chosenAssembler->msil->create_property(sp, getter, setter);
            sp->has_property_setter = TRUE;
        }
    }
    return lex;
}
