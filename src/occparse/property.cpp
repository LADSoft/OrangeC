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
#include "Utils.h"
#include "ccerr.h"
#include "config.h"
#include "initbackend.h"
#include "stmt.h"
#include "ildata.h"
#include "expr.h"
#include "mangle.h"
#include "declare.h"
#include "memory.h"
#include "ccerr.h"
#include "Property.h"
#include "lex.h"
#include "help.h"
#include "istmt.h"
#include "init.h"
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "namespace.h"
#include "symtab.h"
#include "ListFactory.h"
#include "types.h"

namespace Parser
{
void msilCreateProperty(SYMBOL* s1, SYMBOL* s2, SYMBOL* s3)
{

    Optimizer::MsilProperty prop{Optimizer::SymbolManager::Get(s1), Optimizer::SymbolManager::Get(s2),
                                 Optimizer::SymbolManager::Get(s3)};
    Optimizer::msilProperties.push_back(prop);
}

static SYMBOL* CreateSetterPrototype(SYMBOL* sym)
{
    SYMBOL *rv, *value;
    char name[512];
    sprintf(name, "set_%s", sym->name);
    rv = makeID(sym->sb->storage_class, nullptr, nullptr, litlate(name));
    value = makeID(StorageClass::parameter_, sym->tp, nullptr, "value");
    value->sb->attribs.inheritable.used = true;  // to avoid unused variable errors
    rv->sb->access = AccessLevel::public_;
    rv->tp = Type::MakeType(BasicType::func_, &stdvoid);
    rv->tp->sp = rv;
    rv->tp->syms = symbols->CreateSymbolTable();
    SetLinkerNames(value, Linkage::cdecl_);
    rv->tp->syms->Add(value);
    SetLinkerNames(rv, Linkage::cdecl_);
    return rv;
}
static SYMBOL* CreateGetterPrototype(SYMBOL* sym)
{
    SYMBOL *rv, *nullparam;
    char name[512];
    sprintf(name, "get_%s", sym->name);
    rv = makeID(sym->sb->storage_class, nullptr, nullptr, litlate(name));
    nullparam = makeID(StorageClass::parameter_, &stdvoid, nullptr, "__void");
    rv->sb->access = AccessLevel::public_;
    rv->tp = Type::MakeType(BasicType::func_, sym->tp);
    rv->tp->sp = rv;
    rv->tp->syms = symbols->CreateSymbolTable();
    SetLinkerNames(nullparam, Linkage::cdecl_);
    rv->tp->syms->Add(nullparam);
    SetLinkerNames(rv, Linkage::cdecl_);
    return rv;
}
static void insertfunc(SYMBOL* in, SymbolTable<SYMBOL>* syms)
{

    SYMBOL* funcs = syms->Lookup(in->name);
    if (!funcs)
    {
        auto tp = Type::MakeType(BasicType::aggregate_);
        funcs = makeID(StorageClass::overloads_, tp, 0, litlate(in->name));
        tp->sp = funcs;
        SetLinkerNames(funcs, Linkage::cdecl_);
        syms->Add(funcs);
        funcs->tp->syms = symbols->CreateSymbolTable();
        funcs->tp->syms->Add(in);
        in->sb->overloadName = funcs;
    }
    else if (funcs->sb->storage_class == StorageClass::overloads_)
    {
        funcs->tp->syms->insertOverload(in);
        in->sb->overloadName = funcs;
    }
    else
    {
        Utils::Fatal("insertfuncs: invalid overload tab");
    }
}
static SYMBOL* CreateBackingVariable(SYMBOL* sym)
{
    char name[512];
    SYMBOL* rv;
    sprintf(name, "__backing_%s", sym->name);
    rv = makeID(StorageClass::static_, sym->tp, nullptr, litlate(name));
    rv->sb->label = Optimizer::nextLabel++;
    SetLinkerNames(rv, Linkage::cdecl_);
    return rv;
}
static SYMBOL* CreateBackingSetter(SYMBOL* sym, SYMBOL* backing)
{
    SYMBOL* p = CreateSetterPrototype(sym);
    Statement* st;
    FunctionBlock bd = {};
    std::list<FunctionBlock*> b = {&bd};
    EXPRESSION* left = MakeExpression(ExpressionNode::global_, backing);
    EXPRESSION* right = MakeExpression(ExpressionNode::global_, (SYMBOL*)p->tp->syms->front());
    p->tp->type = BasicType::ifunc_;
    memset(&b, 0, sizeof(b));
    Dereference(sym->tp, &left);
    Dereference(sym->tp, &right);
    st = Statement::MakeStatement(nullptr, b, StatementNode::expr_);
    st->select = MakeExpression(ExpressionNode::assign_, left, right);
    p->sb->inlineFunc.stmt = stmtListFactory.CreateList();
    p->sb->inlineFunc.stmt->push_back(Statement::MakeStatement(nullptr, emptyBlockdata, StatementNode::block_));
    p->sb->inlineFunc.stmt->front()->lower = bd.statements;
    p->sb->inlineFunc.syms = p->tp->syms;
    return p;
}
static SYMBOL* CreateBackingGetter(SYMBOL* sym, SYMBOL* backing)
{
    SYMBOL* p = CreateGetterPrototype(sym);
    Statement* st;
    FunctionBlock bd = {};
    std::list<FunctionBlock*> b{&bd};
    p->tp->type = BasicType::ifunc_;
    memset(&b, 0, sizeof(b));
    st = Statement::MakeStatement(nullptr, b, StatementNode::return_);
    st->select = MakeExpression(ExpressionNode::global_, backing);
    Dereference(sym->tp, &st->select);
    p->sb->inlineFunc.stmt = stmtListFactory.CreateList();
    p->sb->inlineFunc.stmt->push_back(Statement::MakeStatement(nullptr, emptyBlockdata, StatementNode::block_));
    p->sb->inlineFunc.stmt->front()->lower = bd.statements;
    p->sb->inlineFunc.syms = p->tp->syms;
    return p;
}
LexList* initialize_property(LexList* lex, SYMBOL* funcsp, SYMBOL* sym, StorageClass storage_class_in, bool asExpression, int flags)
{
    if (sym->tp->IsStructured())
        error(ERR_ONLY_SIMPLE_PROPERTIES_SUPPORTED);
    if (funcsp || sym->sb->storage_class == StorageClass::parameter_)
        error(ERR_NO_PROPERTY_IN_FUNCTION);
    if (sym->sb->storage_class != StorageClass::external_)
    {
        if (MATCHKW(lex, Keyword::begin_))
        {
            SYMBOL *get = nullptr, *set = nullptr;
            SYMBOL* prototype = nullptr;
            lex = getsym();
            while (ISID(lex))
            {
                bool err = false;
                if (!strcmp(lex->data->value.s.a, "get"))
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
                else if (!strcmp(lex->data->value.s.a, "set"))
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
                    needkw(&lex, Keyword::begin_);
                    errskim(&lex, skim_end);
                    needkw(&lex, Keyword::end_);
                }
                else
                {
                    StatementGenerator sg(lex, prototype);
                    sg.FunctionBody();
                    sg.BodyGen();
                    insertfunc(prototype, globalNameSpace->front()->syms);
                }
            }
            if (!get)
                errorsym(ERR_MUST_DECLARE_PROPERTY_GETTER, sym);
            if (set)
                sym->sb->has_property_setter = true;
            msilCreateProperty(sym, get, set);
            needkw(&lex, Keyword::end_);
        }
        else  // create default getter and setter
        {
            SYMBOL* backing = CreateBackingVariable(sym);
            SYMBOL* setter = CreateBackingSetter(sym, backing);
            SYMBOL* getter = CreateBackingGetter(sym, backing);
            Optimizer::SymbolManager::Get(backing);
            Optimizer::SymbolManager::Get(setter);
            Optimizer::SymbolManager::Get(getter);
            insertfunc(setter, globalNameSpace->front()->syms);
            insertfunc(getter, globalNameSpace->front()->syms);
            globalNameSpace->front()->syms->Add(backing);
            if (!TotalErrors())
            {
                int oldstartlab = startlab;
                int oldretlab = retlab;
                startlab = Optimizer::nextLabel++;
                retlab = Optimizer::nextLabel++;
                genfunc(getter, true);
                retlab = oldretlab;
                startlab = oldstartlab;
            }
            if (!TotalErrors())
            {
                int oldstartlab = startlab;
                int oldretlab = retlab;
                startlab = Optimizer::nextLabel++;
                retlab = Optimizer::nextLabel++;
                genfunc(setter, true);
                retlab = oldretlab;
                startlab = oldstartlab;
            }
            insertInitSym(backing);
            msilCreateProperty(sym, getter, setter);
            sym->sb->has_property_setter = true;
        }
        InsertGlobal(sym);
    }
    return lex;
}
Type* find_boxed_type(Type* in)
{
    static const char* typeNames[] = {"int8",   "Bool",   "Int8",  "Int8",   "UInt8",  "Uint8",  "Int16",   "Int16",  "UInt16",
                                      "UInt16", "Int32",  "Int32", "IntPtr", "Int32",  "UInt32", "UIntPtr", "Int32",  "UInt32",
                                      "Int64",  "UInt64", "",      "",       "Single", "Double", "Double",  "Single", "Double",
                                      "Double", "",       "",      "",       "",       "",       "String"};
    if (in->BaseType()->IsArray() && in->BaseType()->msil)
    {
        SYMBOL* sym = search(globalNameSpace->front()->syms, "System");
        if (sym && sym->sb->storage_class == StorageClass::namespace_)
        {
            SYMBOL* sym2 = search(sym->sb->nameSpaceValues->front()->syms, "Array");
            if (sym2)
                return sym2->tp;
        }
    }
    else if ((int)in->BaseType()->type < sizeof(typeNames) / sizeof(typeNames[0]))
    {
        SYMBOL* sym = search(globalNameSpace->front()->syms, "System");
        if (sym && sym->sb->storage_class == StorageClass::namespace_)
        {
            SYMBOL* sym2 = search(sym->sb->nameSpaceValues->front()->syms, typeNames[(int)in->BaseType()->type]);
            if (sym2)
                return sym2->tp;
        }
    }
    return nullptr;
}
Type* find_unboxed_type(Type* in)
{
    if (in->IsStructured())
    {
        in = in->BaseType();
        if (in->sp->sb->parentNameSpace && !in->sp->sb->parentClass && !strcmp(in->sp->sb->parentNameSpace->name, "System"))
        {
            const char* name = in->sp->name;
            static const char* typeNames[] = {"Bool",  "Char",   "Int8",   "UInt8",   "Int16",  "UInt16", "Int32", "UInt32",
                                              "Int64", "UInt64", "IntPtr", "UIntPtr", "Single", "Double", "String"};
            static Type* typeVals[] = {&stdbool,          &stdchar,    &stdchar,     &stdunsignedchar, &stdshort,
                                       &stdunsignedshort, &stdint,     &stdunsigned, &stdlonglong,     &stdunsignedlonglong,
                                       &stdinative,       &stdunative, &stdfloat,    &stddouble,       &std__string};
            for (int i = 0; i < sizeof(typeNames) / sizeof(typeNames[0]); i++)
                if (!strcmp(typeNames[i], name))
                {
                    return typeVals[i];
                }
        }
    }
    return nullptr;
}
}  // namespace Parser