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
    extern BOOLEAN managed_library;
}

void AddType(SYMBOL *sym, Type *type);

class Importer :public Callback
{
public:
    Importer() : level_(0), inlsmsilcrtl(0) { }
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
    bool useGlobal() const { return managed_library && inlsmsilcrtl && structures_.size() == 1; }
private:
    std::deque<SYMBOL *> nameSpaces_;
    std::deque<SYMBOL *> structures_;
    std::map<std::string, SYMBOL *> cachedClasses;
    int level_;
    bool inlsmsilcrtl;
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
    bt_inative, bt_unative,
    bt_float, bt_double, bt___object, bt___string

};

TYPE *Importer::TranslateType(Type *in)
{
    TYPE *rv = NULL;
    if (in)
    {
        enum e_bt tp;
        if (in->ArrayLevel())
            return NULL;
        if (in->GetBasicType() == Type::cls)
        {
            SYMBOL *sp = cachedClasses[in->GetClass()->Name()];
            if (!sp)
                return nullptr;
            rv = sp->tp;
        }
        else
        {
            tp = translatedTypes[in->GetBasicType()];
            if (tp == bt_void && in->GetBasicType() != Type::Void)
                return nullptr;
            rv = (TYPE *)Alloc(sizeof(TYPE));
            rv->type = tp;
            rv->size = 1;
        }
        for (int i=0; i < in->PointerLevel(); i++)
        {
            TYPE *tp1 = (TYPE *)Alloc(sizeof(TYPE));
            tp1->type = bt_pointer;
            tp1->size = getSize(bt_int);
            tp1->btp = rv;
            rv = tp1;
        }
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
        if (nameSpaces_.size())
        {
            sp->parentNameSpace = nameSpaces_.back();
        }
        SetLinkerNames(sp, lk_none);
        if (nameSpaces_.size() == 0)
        {
            insert(sp, globalNameSpace->syms);
            insert(sp, globalNameSpace->tags);
        }
        else
        {
            insert(sp, nameSpaces_.back()->nameSpaceValues->syms);
            insert(sp, nameSpaces_.back()->nameSpaceValues->tags);
        }
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
    if (structures_.size() == 0 && nameSpaces_.size() == 1)
    {
        inlsmsilcrtl = cls->Name() == "rtl" && !strcmp(nameSpaces_.back()->name, "lsmsilcrtl");
    }
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
            sp->tp->size = 1;// needs to be NZ but we don't really care what is is in the MSIL compiler
            sp->tp->syms = CreateHashTable(1);
            sp->tp->rootType = sp->tp;
            sp->tp->sp = sp;
            sp->declfile = sp->origdeclfile = "[import]";
            sp->trivialCons = TRUE;
            if (structures_.size())
                sp->parentClass = structures_.back();
            if (nameSpaces_.size())
                sp->parentNameSpace = nameSpaces_.back();
            sp->access = ac_public;
            SetLinkerNames(sp, lk_cdecl);

            if (useGlobal())
                insert(sp, globalNameSpace->syms);
            else
                insert(sp, structures_.size() ? structures_.back()->tp->syms : nameSpaces_.back()->nameSpaceValues->syms);
            sp->msil = (void *)cls;
            AddType(sp, peLib->AllocateType(const_cast<Class *>(cls)));
            cachedClasses[sp->name] = sp;
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
    if (!structures_.size())
        inlsmsilcrtl = FALSE;
    return true;
}
bool Importer::EnterMethod(const Method *method)
{
    diag("Method", method->Signature()->Name());
    if (structures_.size())
    {
        bool ctor = false;
        int count = method->Signature()->ParamCount();
        if (method->Signature()->Flags() & MethodSignature::Vararg)
            count --;
        if (method->Signature()->Name() == ".ctor")
        {
            ctor = TRUE;
            structures_.back()->trivialCons = FALSE;
        }
        TYPE *tp = (TYPE *)Alloc(sizeof(TYPE));
        std::vector<TYPE *>args;
        std::vector<std::string> names;
        tp->type = bt_func;
        tp->btp = TranslateType(method->Signature()->ReturnType());
        if (tp->btp)
        {
            int n = 0;
            for (auto it = method->Signature()->begin(); it != method->Signature()->end(); ++it)
            {
                if (!count) // vararg
                {
                    if ((*it)->GetType()->GetBasicType() != Type::object || (*it)->GetType()->ArrayLevel() != 1)
                        tp = NULL;
                    break;
                }
                else 
                {
                    TYPE *tp1 = TranslateType((*it)->GetType());
                    if (tp1)
                    {
                        char name[256];
                        sprintf(name, "$$Arg%d", n++);
                        args.push_back(tp1);
                        names.push_back(name);
                    }
                    else
                    {
                        tp = NULL;
                        break;
                    }
                }
                count--;
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
            if (!(method->Signature()->Flags() & MethodSignature::InstanceFlag))
                sp->storage_class = sc_static;
            else
                sp->storage_class = sc_member;
            sp->tp = tp;
            sp->tp->sp = sp;
            sp->parentClass = structures_.back();
            sp->declfile = sp->origdeclfile = "[import]";
            sp->access = ac_public;
            sp->tp->syms = CreateHashTable(1);
            sp->isConstructor = ctor;
            if (!args.size())
            {
                TYPE *tp1 = (TYPE *)Alloc(sizeof(TYPE));
                tp1->type = bt_void;
                args.push_back(tp1);
                names.push_back("$$void");
            }
            if (sp->storage_class == sc_member)
            {
                SYMBOL *sp1 = (SYMBOL *)Alloc(sizeof(SYMBOL));
                sp1->name = litlate("$$this");
                sp1->storage_class = sc_parameter;
                sp1->thisPtr = true;
                sp1->tp = (TYPE *)Alloc(sizeof(TYPE));
                sp1->tp->type = bt_pointer;
                sp1->tp->size = getSize(bt_int);
                sp1->tp->btp = structures_.back()->tp;
                sp1->declfile = sp1->origdeclfile = "[import]";
                sp1->access = ac_public;
                SetLinkerNames(sp1, lk_cdecl);
                insert(sp1, sp->tp->syms);
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
            if (method->Signature()->Flags() & MethodSignature::Vararg)
            {
                SYMBOL *sp1 = (SYMBOL *)Alloc(sizeof(SYMBOL));
                sp1->name = litlate((char *)"$$vararg");
                sp1->storage_class = sc_parameter;
                sp1->tp = (TYPE *)Alloc(sizeof(TYPE));
                sp1->tp->type = bt_ellipse; 
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
                if (useGlobal())
                    insert(sp, globalNameSpace->syms);
                else
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
    return true;
}
bool Importer::EnterField(const Field *field)
{
    diag("Field", field->Name());
    if (structures_.size())
    {
        TYPE *tp = TranslateType(field->FieldType());
        if (tp)
        {
            SYMBOL *sp = (SYMBOL *)Alloc(sizeof(SYMBOL));
            sp->name = litlate((char *)field->Name().c_str());
            if (field->Flags().Flags() & Qualifiers::Static)
                sp->storage_class = sc_static;
            else
                sp->storage_class = sc_member;
            sp->tp = tp;
            sp->parentClass = structures_.back();
            sp->declfile = sp->origdeclfile = "[import]";
            sp->access = ac_public;
            sp->msil = (void *)field;
            SetLinkerNames(sp, lk_cdecl);
            if (useGlobal())
                insert(sp, globalNameSpace->syms);
            else
                insert(sp, structures_.back()->tp->syms);
        }
    }
    return true;
}
bool Importer::EnterProperty(const Property *property)
{
    diag("Property", property->Name());
    if (structures_.size())
    {
        TYPE *tp = TranslateType(property->GetType());
        if (tp)
        {
            SYMBOL *sp = (SYMBOL *)Alloc(sizeof(SYMBOL));
            sp->name = litlate((char *)property->Name().c_str());
            if (!property->Instance())
                sp->storage_class = sc_static;
            else
                sp->storage_class = sc_member;
            sp->tp = tp;
            sp->parentClass = structures_.back();
            sp->declfile = sp->origdeclfile = "[import]";
            sp->access = ac_public;
            sp->msil = (void *)property;
            sp->linkage2 = lk_property;
            if (const_cast<Property *>(property)->Setter())
                sp->has_property_setter = TRUE;
            SetLinkerNames(sp, lk_cdecl);
            if (useGlobal())
                insert(sp, globalNameSpace->syms);
            else
                insert(sp, structures_.back()->tp->syms);
        }
    }
    return true;
}
