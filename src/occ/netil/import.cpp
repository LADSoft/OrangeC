/*
Software License Agreement (BSD License)

Copyright (c) 1997-2011, David Lindauer, (LADSoft).
All rights reserved.

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
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include <windows.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "Be.h"
#include "winmode.h"
#include "DotNetpeLib.h"

#include <fstream>
#include <deque>
#include <string>
using namespace DotNetPELib;

extern "C"
{
    extern PELib *peLib;
    extern NAMESPACEVALUES *globalNameSpace;
    extern LIST *nameSpaceList;
}
class Importer :public Callback
{
public:
    Importer() : level_(0) { }
    virtual ~Importer() { }

    virtual bool EnterAssembly(const AssemblyDef *) override;
    virtual bool ExitAssembly(const AssemblyDef *) override;
    virtual bool EnterNamespace(const Namespace *) override;
    virtual bool ExitNamespace(const Namespace *) override;
    virtual bool EnterClass(const Class *) override;
    virtual bool ExitClass(const Class *) override;
    virtual bool EnterEnum(const Enum *cls) override {
        return EnterClass(cls);
    }
    virtual bool ExitEnum(const Enum *cls) override {
        return ExitClass(cls);
    }
    virtual bool EnterMethod(const Method *) override;
    virtual bool EnterField(const Field *) override;
    virtual bool EnterProperty(const Property *) override;

#if 1
#define diag(x,y)
#else
#define diag(x,y) Diag(x,y)
    void Diag(std::string header, std::string name)
    {
        for (int i = 0; i < level_; i++)
            std::cout << "\t";
        if (header.size())
            std::cout << header << ", " << name << std::endl;
        else
            std::cout << name << std::endl;
    }
#endif
protected:
    TYPE *TranslateType(Type *);
private:
    std::deque<SYMBOL *> nameSpaces_;
    std::deque<SYMBOL *> structures_;
    int level_;

    static e_bt translatedTypes[];
};

void Import()
{
    Importer importer;
    peLib->Traverse(importer);
}

e_bt Importer::translatedTypes[] =
{
    ///** type is a reference to a class
    bt_void,
    ///** type is a reference to a method signature
    bt_void,
    /* below this is various CIL types*/
    bt_void, bt_bool, bt_wchar_t, 
    bt_char, bt_unsigned_char, 
    bt_short, bt_unsigned_short,
    bt_int, bt_unsigned ,
    bt_long_long, bt_unsigned_long_long,
    bt_int, bt_unsigned,
    bt_float, bt_double, bt___object, bt___string

};

TYPE *Importer::TranslateType(Type *in)
{
    TYPE *rv = NULL;
    if (in)
    {
        enum e_bt tp;
        if (in->ArrayLevel() || in->PointerLevel())
            return NULL;
        tp = translatedTypes[in->GetBasicType()];
        if (tp == bt_void && in->GetBasicType() != Type::Void)
            return NULL;
        rv = (TYPE *)Alloc(sizeof(TYPE));
        rv->type = tp;
        rv->size = 1;
        if (in->ByRef())
        {
            TYPE *tp1 = (TYPE *)Alloc(sizeof(TYPE));
            tp1->type = bt_lref;
            tp1->size = getSize(bt_int);
            tp1->btp = rv;
            rv = tp1;
        }
    }
    return rv;
}

bool Importer::EnterAssembly(const AssemblyDef *assembly)
{
    if (!assembly->InAssemblyRef())
        return false;
    diag("Assembly", assembly->Name());
    level_++;
    return true;
}
bool Importer::ExitAssembly(const AssemblyDef *assembly) 
{
    level_--;
    diag("Exit Assembly", assembly->Name());
    return true;
}
bool Importer::EnterNamespace(const Namespace *nameSpace)
{
    diag("Namespace", nameSpace->Name());
    level_++;
    HASHREC **hr = LookupName((char *)nameSpace->Name().c_str(), globalNameSpace->syms);
    SYMBOL *sp;
    if (!hr)
    {
        TYPE *tp = (TYPE *)Alloc(sizeof(TYPE));
        tp->type = bt_void;
        sp = makeID(sc_namespace, tp, NULL, litlate((char *)nameSpace->Name().c_str()));
        sp->nameSpaceValues = (NAMESPACEVALUES *)Alloc(sizeof(NAMESPACEVALUES));
        sp->nameSpaceValues->syms = CreateHashTable(GLOBALHASHSIZE);
        sp->nameSpaceValues->tags = CreateHashTable(GLOBALHASHSIZE);
        sp->nameSpaceValues->origname = sp;
        sp->nameSpaceValues->name = sp;
        sp->parentNameSpace = globalNameSpace->name;
        sp->linkage = lk_cdecl;
        sp->msil = (void *)nameSpace;
        if (nameSpaceList)
        {
            sp->parentNameSpace = (SYMBOL *)nameSpaceList->data;
        }
        SetLinkerNames(sp, lk_none);
        insert(sp, globalNameSpace->syms);
        insert(sp, globalNameSpace->tags);
    }
    else
    {
        sp = (SYMBOL *)(*hr)->p;
        if (sp->storage_class != sc_namespace)
        {
            fatal("internal error: misuse of namespace");
        }
    }
    sp->value.i++;
    nameSpaces_.push_back(sp);
    return true;
}
bool Importer::ExitNamespace(const Namespace *nameSpace)
{
    level_--;
    diag("Exit Namespace", nameSpace->Name());
    nameSpaces_.pop_back();
    return true;
}
bool Importer::EnterClass(const Class *cls)
{
    diag("Class", cls->Name());
    level_++;
    if (nameSpaces_.size())
    {
        SYMBOL *sp = NULL;
        HASHREC **hr;
        if (structures_.size())
        {
            hr = LookupName((char *)cls->Name().c_str(), structures_.back()->tp->syms);
        }
        else if (nameSpaces_.size())
        {
            hr = LookupName((char *)cls->Name().c_str(), nameSpaces_.back()->nameSpaceValues->syms);
        }
        if (hr)
            sp = (SYMBOL *)(*hr)->p;
        if (!sp)
        {
            sp = (SYMBOL *)Alloc(sizeof(SYMBOL));
            sp->name = litlate((char *)cls->Name().c_str());
            sp->storage_class = sc_type;
            sp->tp = (TYPE *)Alloc(sizeof(TYPE));
            sp->tp->type = bt_struct;
            sp->tp->syms = CreateHashTable(1);
            sp->tp->rootType = sp->tp;
            sp->tp->sp = sp;
            sp->declfile = sp->origdeclfile = "[import]";
            if (structures_.size())
                sp->parentClass = structures_.back();
            if (nameSpaces_.size())
                sp->parentNameSpace = nameSpaces_.back();
            sp->access = ac_public;
            SetLinkerNames(sp, lk_cdecl);

            insert(sp, structures_.size() ? structures_.back()->tp->syms : nameSpaces_.back()->nameSpaceValues->syms);
            sp->msil = (void *)cls;
        }
        else
        {
            if (!isstructured(sp->tp))
            {
                fatal("internal error: misuse of class");
            }
        }
        structures_.push_back(sp);
    }
    return true;
}
bool Importer::ExitClass(const Class *cls)
{
    level_--;
    diag("Exit Class", cls->Name());
    structures_.pop_back();
    return true;
}
bool Importer::EnterMethod(const Method *method)
{
    diag("Method", method->Signature()->Name());
    if (structures_.size())
    {
        if (!(method->Signature()->Flags() & MethodSignature::InstanceFlag))
        {
            if (!(method->Signature()->Flags() & MethodSignature::Vararg))
            {
                // static instance member with no variable length argument list is all we support right now...
                TYPE *tp = (TYPE *)Alloc(sizeof(TYPE));
                std::vector<TYPE *>args;
                std::vector<std::string> names;
                tp->type = bt_func;
                tp->btp = TranslateType(method->Signature()->ReturnType());
                if (tp->btp)
                {
                    for (auto it = method->Signature()->begin(); it != method->Signature()->end(); ++it)
                    {
                        TYPE *tp1 = TranslateType((*it)->GetType());
                        if (tp1)
                        {
                            args.push_back(tp1);
                            names.push_back((*it)->Name());
                        }
                        else
                        {
                            tp = NULL;
                            break;
                        }
                    }

                }
                else
                {
                    tp = NULL;
                }
                if (tp)
                {
                    SYMBOL *sp = (SYMBOL *)Alloc(sizeof(SYMBOL));
                    sp->name = litlate((char *)method->Signature()->Name().c_str());
                    sp->storage_class = sc_static;
                    sp->tp = tp;
                    sp->tp->sp = sp;
                    sp->declfile = sp->origdeclfile = "[import]";
                    sp->access = ac_public;
                    sp->tp->syms = CreateHashTable(1);
                    if (!args.size())
                    {
                        TYPE *tp1 = (TYPE *)Alloc(sizeof(TYPE));
                        tp1->type = bt_void;
                        args.push_back(tp1);
                        names.push_back("$$void");
                    }
                    for (int i = 0; i < args.size(); i++)
                    {
                        SYMBOL *sp1 = (SYMBOL *)Alloc(sizeof(SYMBOL));
                        sp1->name = litlate((char *)names[i].c_str());
                        sp1->storage_class = sc_parameter;
                        sp1->tp = args[i];
                        sp1->declfile = sp1->origdeclfile = "[import]";
                        sp1->access = ac_public;
                        SetLinkerNames(sp1, lk_cdecl);
                        insert(sp1, sp->tp->syms);
                    }
                    SetLinkerNames(sp, lk_cdecl);

                    HASHREC **hr = LookupName((char *)method->Signature()->Name().c_str(), structures_.back()->tp->syms);
                    SYMBOL *funcs = NULL;
                    if (hr)
                        funcs = (SYMBOL *)((*hr)->p);
                    if (!funcs)
                    {
                        TYPE *tp = (TYPE *)Alloc(sizeof(TYPE));
                        tp->type = bt_aggregate;
                        tp->rootType = tp;
                        funcs = makeID(sc_overloads, tp, 0, litlate((char *)method->Signature()->Name().c_str()));
                        funcs->parentClass = structures_.back();
                        tp->sp = funcs;
                        SetLinkerNames(funcs, lk_cdecl);
                        insert(funcs, structures_.back()->tp->syms);
                        funcs->parent = sp;
                        funcs->tp->syms = CreateHashTable(1);
                        insert(sp, funcs->tp->syms);
                        sp->overloadName = funcs;
                    }
                    else if (funcs->storage_class == sc_overloads)
                    {
                        insertOverload(sp, funcs->tp->syms);
                        sp->overloadName = funcs;
                    }
                    else
                    {
                        fatal("backend: invalid overload tab");
                    }
                    sp->msil = (void *)method;
                }
            }
        }
    }
    return true;
}
bool Importer::EnterField(const Field *field)
{
    diag("Field", field->Name());
    if (field->Flags().Flags() & Qualifiers::Static)
    {
    }
    return true;
}
bool Importer::EnterProperty(const Property *property)
{
    diag("Property", property->Name());
    return true;
}
