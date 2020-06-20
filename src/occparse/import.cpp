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

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"
#include "winmode.h"
#include "DotNetPELib.h"
#include "Utils.h"

#include <fstream>
#include <deque>
#include <string>
#include <algorithm>
#include "import.h"
#include "symtab.h"
#include "declcpp.h"
#include "initbackend.h"
#include "occparse.h"
#include "memory.h"
#include "help.h"
#include "declare.h"
#include "mangle.h"
#include "beinterf.h"

extern DotNetPELib::PELib* peLib;

using namespace DotNetPELib;

namespace Parser
{

class Importer : public Callback
{
  public:
    Importer() : level_(0), inlsmsilcrtl_(0), pass_(0) {}
    virtual ~Importer() {}

    virtual bool EnterAssembly(const AssemblyDef*) override;
    virtual bool ExitAssembly(const AssemblyDef*) override;
    virtual bool EnterNamespace(const Namespace*) override;
    virtual bool ExitNamespace(const Namespace*) override;
    virtual bool EnterClass(const Class*) override;
    virtual bool ExitClass(const Class*) override;
    virtual bool EnterEnum(const Enum* cls) override { return EnterClass(cls); }
    virtual bool ExitEnum(const Enum* cls) override { return ExitClass(cls); }
    virtual bool EnterMethod(const Method*) override;
    virtual bool EnterField(const Field*) override;
    virtual bool EnterProperty(const Property*) override;
    void Pass(int p) { pass_ = p; }
    void Rundown();

#if 1
#    define diag(x, y)
#else
#    define diag(x, y) Diag(x, y)
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
    TYPE* TranslateType(Type*);
    bool useGlobal() const { return Optimizer::cparams.managed_library && inlsmsilcrtl_ && structures_.size() == 1; }
    void InsertBaseClassTree(SYMBOL* sp, const Class* cls);
    void InsertBaseClass(SYMBOL* sp, Class* cls);
    void InsertFuncs(SYMBOL* sp, SYMBOL* base);
    const char* GetName(const DataContainer* container, std::string name = "")
    {
        std::string rv = Qualifiers::GetName("", container, false);
        std::replace(rv.begin(), rv.end(), '/', '.');
        if (name.size())
            rv += "." + name;
        return litlate(rv.c_str());
    }

  private:
    std::deque<SYMBOL*> nameSpaces_;
    std::deque<SYMBOL*> structures_;
    std::map<std::string, SYMBOL*> cachedClasses_;
    int level_;
    int pass_;
    bool inlsmsilcrtl_;
    static e_bt translatedTypes[];
};

#ifdef VSIDE
void AddType(Optimizer::SimpleSymbol* sym, Type* type)
{
#    ifdef ISPARSER
#    endif
}
#endif
}  // namespace Parser
#ifndef VSIDE
namespace occmsil
{
void AddType(Optimizer::SimpleSymbol* sym, Type* type);
}
using namespace occmsil;
#endif

void Import()
{
    using namespace Parser;
    Importer importer;
    importer.Pass(1);
    peLib->Traverse(importer);
    importer.Pass(2);
    peLib->Traverse(importer);
    importer.Pass(3);
    peLib->Traverse(importer);
}

namespace Parser
{
e_bt Importer::translatedTypes[] = {
    ///** type is a reference to a class
    bt_void,
    ///** type is a reference to a method signature
    bt_void,
    ///** type is a var
    bt_void,
    ///** type is an mvar
    bt_void,
    /* below this is various CIL types*/
    bt_void, bt_bool, bt_wchar_t, bt_char, bt_unsigned_char, bt_short, bt_unsigned_short, bt_int, bt_unsigned, bt_long_long,
    bt_unsigned_long_long, bt_inative, bt_unative, bt_float, bt_double, bt___object, bt___string

};

TYPE* Importer::TranslateType(Type* in)
{
    TYPE *rv = NULL, **last = &rv;
    if (in)
    {
        enum e_bt tp;
        if (in->ArrayLevel())
        {
            if (in->ArrayLevel() > 1)  // this is because with the current version of dotnetpelib we don't know the index ranges
                return NULL;
            *last = (TYPE*)Alloc(sizeof(TYPE));
            (*last)->type = bt_pointer;
            (*last)->rootType = (*last);
            (*last)->array = true;
            (*last)->msil = true;  // arrays are always MSIL when imported from an assembly

            last = &(*last)->btp;
        }
        if (in->GetBasicType() == Type::cls)
        {
            SYMBOL* sp = cachedClasses_[in->GetClass()->Name()];
            if (!sp)
                return nullptr;
            if (in->ArrayLevel())
            {
                (*last) = (TYPE*)Alloc(sizeof(TYPE));
                **last = *sp->tp;
                (*last)->msil = true;
            }
            else
            {
                (*last) = sp->tp;
            }
        }
        else
        {
            tp = translatedTypes[in->GetBasicType()];
            if (tp == bt_void && in->GetBasicType() != Type::Void)
                return nullptr;
            (*last) = (TYPE*)Alloc(sizeof(TYPE));
            (*last)->type = tp;
            (*last)->size = 1;
            if (in->ArrayLevel() || tp == bt___string || tp == bt___object)
            {
                (*last)->msil = true;
            }
        }
        for (int i = 0; i < in->PointerLevel(); i++)
        {
            TYPE* tp1 = (TYPE*)Alloc(sizeof(TYPE));
            tp1->type = bt_pointer;
            tp1->rootType = tp1;
            tp1->size = getSize(bt_int);
            tp1->btp = rv;
            rv = tp1;
        }
        if (in->ByRef())
        {
            TYPE* tp1 = (TYPE*)Alloc(sizeof(TYPE));
            tp1->type = bt_lref;
            tp1->size = getSize(bt_int);
            tp1->btp = rv;
            rv = tp1;
        }
    }
    UpdateRootTypes(rv);
    return rv;
}

bool Importer::EnterAssembly(const AssemblyDef* assembly)
{
    if (!assembly->InAssemblyRef())
        return false;
    diag("Assembly", assembly->Name());
    level_++;
    return true;
}
bool Importer::ExitAssembly(const AssemblyDef* assembly)
{
    level_--;
    diag("Exit Assembly", assembly->Name());
    return true;
}
bool Importer::EnterNamespace(const Namespace* nameSpace)
{
    diag("Namespace", nameSpace->Name());
    level_++;
    SYMLIST** hr = LookupName((char*)nameSpace->Name().c_str(), nameSpaces_.size() == 0
                                                                    ? globalNameSpace->valueData->syms
                                                                    : nameSpaces_.back()->sb->nameSpaceValues->valueData->syms);
    SYMBOL* sp;
    if (!hr)
    {
        TYPE* tp = (TYPE*)Alloc(sizeof(TYPE));
        tp->type = bt_void;
        tp->rootType = tp;
        sp = makeID(sc_namespace, tp, NULL, litlate((char*)nameSpace->Name().c_str()));
        sp->sb->nameSpaceValues = (NAMESPACEVALUELIST*)Alloc(sizeof(NAMESPACEVALUELIST));
        sp->sb->nameSpaceValues->valueData = (NAMESPACEVALUEDATA*)Alloc(sizeof(NAMESPACEVALUEDATA));
        sp->sb->nameSpaceValues->valueData->syms = CreateHashTable(GLOBALHASHSIZE);
        sp->sb->nameSpaceValues->valueData->tags = CreateHashTable(GLOBALHASHSIZE);
        sp->sb->nameSpaceValues->valueData->origname = sp;
        sp->sb->nameSpaceValues->valueData->name = sp;
        sp->sb->parentNameSpace = globalNameSpace->valueData->name;
        sp->sb->attribs.inheritable.linkage = lk_cdecl;
        sp->sb->msil = GetName(nameSpace);
        if (nameSpaces_.size())
        {
            sp->sb->parentNameSpace = nameSpaces_.back();
        }
        SetLinkerNames(sp, lk_none);
        if (nameSpaces_.size() == 0)
        {
            insert(sp, globalNameSpace->valueData->syms);
            insert(sp, globalNameSpace->valueData->tags);
        }
        else
        {
            insert(sp, nameSpaces_.back()->sb->nameSpaceValues->valueData->syms);
            insert(sp, nameSpaces_.back()->sb->nameSpaceValues->valueData->tags);
        }
    }
    else
    {
        sp = (SYMBOL*)(*hr)->p;
        if (sp->sb->storage_class != sc_namespace)
        {
            Utils::fatal("internal error: misuse of namespace");
        }
    }
    sp->sb->value.i++;
    nameSpaces_.push_back(sp);
    return true;
}
bool Importer::ExitNamespace(const Namespace* nameSpace)
{
    level_--;
    diag("Exit Namespace", nameSpace->Name());
    nameSpaces_.pop_back();
    return true;
}
bool Importer::EnterClass(const Class* cls)
{
    diag("Class", cls->Name());
    level_++;
    if (structures_.size() == 0 && nameSpaces_.size() == 1)
    {
        inlsmsilcrtl_ = cls->Name() == "rtl" && !strcmp(nameSpaces_.back()->name, "lsmsilcrtl");
    }
    if (nameSpaces_.size())
    {
        SYMBOL* sp = NULL;
        SYMLIST** hr = nullptr;
        if (structures_.size())
        {
            hr = LookupName((char*)cls->Name().c_str(), structures_.back()->tp->syms);
        }
        else if (nameSpaces_.size())
        {
            hr = LookupName((char*)cls->Name().c_str(), nameSpaces_.back()->sb->nameSpaceValues->valueData->syms);
        }
        if (hr)
            sp = (SYMBOL*)(*hr)->p;
        if (!sp)
        {
            sp = SymAlloc();
            sp->name = litlate((char*)cls->Name().c_str());
            sp->sb->storage_class = sc_type;
            sp->tp = (TYPE*)Alloc(sizeof(TYPE));
            if (typeid(*cls) == typeid(Enum))
            {
                sp->tp->type = bt_enum;
                sp->tp->size = getSize(bt_int);
                sp->tp->btp = &stdint;
            }
            else
            {
                sp->tp->type = bt_struct;
                sp->tp->size = 1;  // needs to be NZ but we don't really care what is is in the MSIL compiler
                sp->sb->trivialCons = true;
            }
            if (structures_.size())
                sp->sb->parentClass = structures_.back();
            sp->tp->syms = CreateHashTable(1);
            sp->tp->rootType = sp->tp;
            sp->tp->sp = sp;
            sp->sb->declfile = sp->sb->origdeclfile = "[import]";
            if (nameSpaces_.size())
                sp->sb->parentNameSpace = nameSpaces_.back();
            sp->sb->access = ac_public;
            SetLinkerNames(sp, lk_cdecl);

            if (useGlobal())
                insert(sp, globalNameSpace->valueData->syms);
            else
                insert(sp, structures_.size() ? structures_.back()->tp->syms
                                              : nameSpaces_.back()->sb->nameSpaceValues->valueData->syms);
            sp->sb->msil = GetName(cls);
            cachedClasses_[sp->name] = sp;
        }
        else
        {
            if (!isstructured(sp->tp) && sp->tp->type != bt_enum)
            {
                Utils::fatal("internal error: misuse of class");
            }
        }
        structures_.push_back(sp);
        if (!cls->Extends())
        {
            if (cls->ExtendsName().size())
            {
                void* extends = nullptr;
                if (PELib::s_class == peLib->Find(cls->ExtendsName(), &extends, nullptr))
                {
                    const_cast<Class*>(cls)->Extends(static_cast<Class*>(extends));
                }
            }
        }
        if (pass_ == 3)
        {
            InsertBaseClassTree(sp, cls);
        }
    }
    return true;
}
bool Importer::ExitClass(const Class* cls)
{
    level_--;
    diag("Exit Class", cls->Name());
    if (pass_ == 3)
    {
        SYMBOL* sp = structures_.back();
        AddType(Optimizer::SymbolManager::Get(sp), peLib->AllocateType(const_cast<Class*>(cls)));
    }
    structures_.pop_back();
    if (!structures_.size())
        inlsmsilcrtl_ = false;
    return true;
}
void Importer::InsertBaseClassTree(SYMBOL* sp, const Class* cls)
{
    bool done = false;
    while (!done)
    {
        done = true;
        if (isstructured(sp->tp))
        {
            if (cls->Extends())
            {
                Class* extends = cls->Extends();
                InsertBaseClass(sp, extends);
                if (extends != cls)
                    done = false;
                cls = extends;
            }
        }
    }
}
void Importer::InsertBaseClass(SYMBOL* sp, Class* cls)
{
    SYMBOL* parent = cachedClasses_[cls->Name()];
    if (parent != sp && parent != NULL)  // object is parent to itself
    {
        BASECLASS* srch = sp->sb->baseClasses;
        while (srch)
        {
            if (srch->cls && !strcmp(srch->cls->name, parent->name))
                return;
            srch = srch->next;
        }
        BASECLASS* cl = (BASECLASS*)Alloc(sizeof(BASECLASS));
        cl->accessLevel = ac_public;
        cl->cls = parent;
        cl->next = sp->sb->baseClasses;
        sp->sb->baseClasses = cl;
        InsertFuncs(sp, parent);
    }
}
void Importer::InsertFuncs(SYMBOL* sp, SYMBOL* base)
{
    SYMLIST* hr = base->tp->syms->table[0];
    while (hr)
    {
        SYMBOL* sym = (SYMBOL*)hr->p;
        // inserts an overload list, if there is not already an overload list for this func
        if (sym->sb->storage_class == sc_overloads)
        {
            SYMLIST* hrc = sp->tp->syms->table[0];
            while (hrc)
            {
                if (!strcmp(hrc->p->name, sym->name))
                    break;
                hrc = hrc->next;
            }
            if (!hrc)
            {
                insert(sym, sp->tp->syms);
            }
        }
        hr = hr->next;
    }
}
bool Importer::EnterMethod(const Method* method)
{
    diag("Method", method->Signature()->Name());
    if (pass_ == 2 && structures_.size())
    {
        bool ctor = false;
        int count = method->Signature()->ParamCount();
        if (method->Signature()->Flags() & MethodSignature::Vararg)
            count--;
        if (method->Signature()->Name() == ".ctor")
        {
            ctor = true;
            structures_.back()->sb->trivialCons = false;
            structures_.back()->sb->hasUserCons = true;
        }
        TYPE* tp = (TYPE*)Alloc(sizeof(TYPE));
        std::vector<TYPE*> args;
        std::vector<std::string> names;
        tp->type = bt_func;
        tp->btp = TranslateType(method->Signature()->ReturnType());
        if (tp->btp)
        {
            int n = 0;
            for (auto it = method->Signature()->begin(); it != method->Signature()->end(); ++it)
            {
                if (!count)  // vararg
                {
                    if ((*it)->GetType()->GetBasicType() != Type::object || (*it)->GetType()->ArrayLevel() != 1)
                        tp = NULL;
                    break;
                }
                else
                {
                    TYPE* tp1 = TranslateType((*it)->GetType());
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
            SYMBOL* sp = SymAlloc();
            sp->name = litlate((char*)method->Signature()->Name().c_str());
            if (method->Signature()->Flags() & Qualifiers::Virtual)
                sp->sb->storage_class = sc_virtual;
            else if (!(method->Signature()->Flags() & MethodSignature::InstanceFlag))
                sp->sb->storage_class = sc_static;
            else
                sp->sb->storage_class = sc_member;
            sp->tp = tp;
            sp->tp->sp = sp;
            sp->sb->msil = GetName(method->GetContainer(), method->Signature()->Name());
            sp->sb->parentClass = structures_.back();
            sp->sb->declfile = sp->sb->origdeclfile = "[import]";
            sp->sb->access = ac_public;
            sp->tp->syms = CreateHashTable(1);
            sp->sb->isConstructor = ctor;
            if (!args.size())
            {
                TYPE* tp1 = (TYPE*)Alloc(sizeof(TYPE));
                tp1->type = bt_void;
                tp1->rootType = tp1;
                args.push_back(tp1);
                names.push_back("$$void");
            }
            if (sp->sb->storage_class == sc_member || sp->sb->storage_class == sc_virtual)
            {
                SYMBOL* sp1 = SymAlloc();
                sp1->name = litlate("$$this");
                sp1->sb->storage_class = sc_parameter;
                sp1->sb->thisPtr = true;
                sp1->tp = (TYPE*)Alloc(sizeof(TYPE));
                sp1->tp->type = bt_pointer;
                sp1->tp->rootType = sp1->tp;
                sp1->tp->size = getSize(bt_int);
                sp1->tp->btp = structures_.back()->tp;
                sp1->sb->declfile = sp1->sb->origdeclfile = "[import]";
                sp1->sb->access = ac_public;
                sp1->sb->parent = sp;
                SetLinkerNames(sp1, lk_cdecl);
                insert(sp1, sp->tp->syms);
            }
            for (int i = 0; i < args.size(); i++)
            {
                SYMBOL* sp1 = SymAlloc();
                sp1->name = litlate((char*)names[i].c_str());
                sp1->sb->storage_class = sc_parameter;
                sp1->tp = args[i];
                sp1->sb->declfile = sp1->sb->origdeclfile = "[import]";
                sp1->sb->access = ac_public;
                sp1->sb->parent = sp;
                SetLinkerNames(sp1, lk_cdecl);
                insert(sp1, sp->tp->syms);
            }
            if (method->Signature()->Flags() & MethodSignature::Vararg)
            {
                SYMBOL* sp1 = SymAlloc();
                sp1->name = litlate((char*)"$$vararg");
                sp1->sb->storage_class = sc_parameter;
                sp1->tp = (TYPE*)Alloc(sizeof(TYPE));
                sp1->tp->type = bt_ellipse;
                sp1->sb->declfile = sp1->sb->origdeclfile = "[import]";
                sp1->sb->access = ac_public;
                sp1->sb->parent = sp;
                SetLinkerNames(sp1, lk_cdecl);
                insert(sp1, sp->tp->syms);
            }
            SetLinkerNames(sp, lk_cdecl);

            SYMLIST** hr = LookupName((char*)method->Signature()->Name().c_str(), structures_.back()->tp->syms);
            SYMBOL* funcs = NULL;
            if (hr)
                funcs = (SYMBOL*)((*hr)->p);
            if (!funcs)
            {
                TYPE* tp = (TYPE*)Alloc(sizeof(TYPE));
                tp->type = bt_aggregate;
                tp->rootType = tp;
                funcs = makeID(sc_overloads, tp, 0, litlate((char*)method->Signature()->Name().c_str()));
                funcs->sb->parentClass = structures_.back();
                tp->sp = funcs;
                SetLinkerNames(funcs, lk_cdecl);
                if (useGlobal())
                    insert(funcs, globalNameSpace->valueData->syms);
                else
                    insert(funcs, structures_.back()->tp->syms);
                funcs->sb->parent = sp;
                funcs->tp->syms = CreateHashTable(1);
                insert(sp, funcs->tp->syms);
                sp->sb->overloadName = funcs;
            }
            else if (funcs->sb->storage_class == sc_overloads)
            {
                insertOverload(sp, funcs->tp->syms);
                sp->sb->overloadName = funcs;
            }
            else
            {
                Utils::fatal("backend: invalid overload tab");
            }
        }
    }
    return true;
}
bool Importer::EnterField(const Field* field)
{
    diag("Field", field->Name());
    if (pass_ == 2 && structures_.size())
    {
        TYPE* tp = TranslateType(field->FieldType());
        if (tp)
        {
            SYMBOL* sp = SymAlloc();
            sp->name = litlate((char*)field->Name().c_str());
            if (structures_.back()->tp->type == bt_enum)
            {
                sp->sb->storage_class = sc_enumconstant;
                tp->scoped = tp->enumConst = true;
                sp->sb->value.i = field->EnumValue();
            }
            else
            {
                if (field->Flags().Flags() & Qualifiers::Static)
                    sp->sb->storage_class = sc_static;
                else
                    sp->sb->storage_class = sc_member;
                sp->sb->msil = GetName(field->GetContainer(), field->Name());
            }
            sp->tp = tp;
            sp->sb->parentClass = structures_.back();
            sp->sb->declfile = sp->sb->origdeclfile = "[import]";
            sp->sb->access = ac_public;
            SetLinkerNames(sp, lk_cdecl);
            if (useGlobal())
                insert(sp, globalNameSpace->valueData->syms);
            else
                insert(sp, structures_.back()->tp->syms);
        }
    }
    return true;
}
bool Importer::EnterProperty(const Property* property)
{
    diag("Property", property->Name());
    if (pass_ == 2 && structures_.size())
    {
        TYPE* tp = TranslateType(property->GetType());
        if (tp)
        {
            SYMBOL* sp = SymAlloc();
            sp->name = litlate((char*)property->Name().c_str());
            if (!property->Instance())
                sp->sb->storage_class = sc_static;
            else
                sp->sb->storage_class = sc_member;
            sp->tp = tp;
            sp->sb->parentClass = structures_.back();
            sp->sb->declfile = sp->sb->origdeclfile = "[import]";
            sp->sb->access = ac_public;
            sp->sb->msil = GetName(property->GetContainer(), property->Name());
            sp->sb->attribs.inheritable.linkage2 = lk_property;
            if (const_cast<Property*>(property)->Setter())
                sp->sb->has_property_setter = true;
            SetLinkerNames(sp, lk_cdecl);
            if (useGlobal())
                insert(sp, globalNameSpace->valueData->syms);
            else
                insert(sp, structures_.back()->tp->syms);
        }
    }
    return true;
}
}  // namespace Parser