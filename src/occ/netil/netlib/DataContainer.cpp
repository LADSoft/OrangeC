/*
    Software License Agreement (BSD License)

    Copyright (c) 2016, David Lindauer, (LADSoft).
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

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#include "DotNetPELib.h"
#include "PEFile.h"
#include <typeinfo>
namespace DotNetPELib
{
    void DataContainer::Add(Field *field)
    {
        if (field)
        {
            field->SetContainer(this);
            fields_.push_back(field);
        }
    }
    size_t DataContainer::ParentNamespace(PELib &peLib) const
    {
        DataContainer *current = this->Parent();
        while (current && typeid(*current) != typeid(Namespace))
            current = current->Parent();
        if (current)
        {
            if (current && current->InAssemblyRef())
                static_cast<Namespace *>(current)->PEDump(peLib);
            return current->PEIndex();
        }
        return 0;
    }
    size_t DataContainer::ParentClass(PELib &peLib) const
    {
        DataContainer *current = Parent();
        if (current && typeid(*current) == typeid(Class))
        {
            if (current && current->InAssemblyRef())
                static_cast<Class *>(current)->PEDump(peLib);
            return current->PEIndex();
        }
        return 0;
    }
    size_t DataContainer::ParentAssembly(PELib &peLib) const
    {
        // the parent assembly is always at top of the datacontainer tree
        DataContainer *current = Parent();
        while (current->Parent() && typeid(*current) != typeid(AssemblyDef))
        {
            current = current->Parent();
        }
        if (current && current->InAssemblyRef())
            static_cast<AssemblyDef *>(current)->PEDump(peLib);
        return current->PEIndex();
    }
    DataContainer *DataContainer::FindContainer(std::vector<std::string>& split, size_t &n)
    {
        n = 0;
        DataContainer *current = this, *rv = current;
        for (int i=0; i < split.size(); i++)
        {
            current = current->FindContainer(split[i]);
            if (!current)
                break;
            rv = current;
            n++;
        }
        return rv;
    }
    bool DataContainer::ILSrcDump(PELib &peLib) const
    {
        for (std::list<Field *>::const_iterator it = fields_.begin(); it != fields_.end(); ++it)
            (*it)->ILSrcDump(peLib);
        for (std::list<CodeContainer *>::const_iterator it = methods_.begin(); it != methods_.end(); ++it)
            (*it)->ILSrcDump(peLib);
        for (std::list<DataContainer *>::const_iterator it = children_.begin(); it != children_.end(); ++it)
            (*it)->ILSrcDump(peLib);
        return true;
    }
    void DataContainer::ObjOut(PELib &peLib, int pass) const
    {
        peLib.Out() << std::endl << "$db";
        if (pass == 2)
        {
            for (auto field : fields_)
                field->ObjOut(peLib, pass);
        }
        if (pass >= 2)
        {
            for (auto method : methods_)
                method->ObjOut(peLib, pass);
        }
        for (auto child : children_)
            child->ObjOut(peLib, pass);
        peLib.Out() << std::endl << "$de";
    }
    void DataContainer::ObjIn(PELib &peLib, bool definition)
    {
        peLib.PushContainer(this);
        if (peLib.ObjBegin() != 'd')
            peLib.ObjError(oe_syntax);
        bool done = false;
        while (!done)
        {
            switch (peLib.ObjBegin())
            {
                case 'n':
                    Add(Namespace::ObjIn(peLib));
                    break;
                case 'E':
                    Add(Enum::ObjIn(peLib));
                    break;
                case 'c':
                    Add(Class::ObjIn(peLib));
                    break;
                case 'f':
                    Add(Field::ObjIn(peLib));
                    break;
                case 'm':
                    Add(Method::ObjIn(peLib));
                    break;
                default:
                    done = true;
                    break;
            }
        }
        if (peLib.ObjEnd(false) != 'd')
            peLib.ObjError(oe_syntax);
        peLib.PopContainer();
    }
    bool DataContainer::PEDump(PELib &peLib)
    {
        for (auto field : fields_)
            field->PEDump(peLib);
        for (auto method : methods_)
            method->PEDump(peLib);
        for (auto child : children_)
            child->PEDump(peLib);
        return true;
    }
    void DataContainer::Number(int &n)
    {
        if (typeid(*this) != typeid(Namespace))
            peIndex_ = n++;
        for (auto child : children_)
            child->Number(n);
    }
    void DataContainer::Compile(PELib &peLib)
    {
        for (auto method : methods_)
            method->Compile(peLib);
        for (auto child : children_)
            child->Compile(peLib);
    }
    void DataContainer::BaseTypes(int &types) const
    {
        for (auto method : methods_)
            method->BaseTypes(types);
        for (auto child : children_)
            child->BaseTypes(types);
        if (typeid(*this) != typeid(PELib))
            if (typeid(*this) == typeid(Enum))
                types |= basetypeEnum;
            else if (typeid(*this) != typeid(Namespace))
                if (flags_.Flags() & Qualifiers::Value)
                    types |= basetypeValue;
                else
                    types |= basetypeObject;
    }
    bool DataContainer::Traverse(Callback &callback) const
    {
        for (auto child : children_)
            if (typeid(*child) == typeid(Class))
            {
                if (!callback.EnterClass(static_cast<const Class *>(child)))
                    continue;
                if (!child->Traverse(callback))
                    return true;
                if (!callback.ExitClass(static_cast<const Class *>(child)))
                    return false;
            }
            else if (typeid(*child) == typeid(Enum))
            {
                if (!callback.EnterEnum(static_cast<const Enum *>(child)))
                    continue;
                if (!child->Traverse(callback))
                    return true;
                if (!callback.ExitEnum(static_cast<const Enum *>(child)))
                    return false;
            }
            else if (typeid(*child) == typeid(Namespace))
            {
                if (!callback.EnterNamespace(static_cast<const Namespace *>(child)))
                    continue;
                if (!child->Traverse(callback))
                    return true;
                if (!callback.ExitNamespace(static_cast<const Namespace *>(child)))
                    return false;
            }
        for (auto field : fields_)
            if (!callback.EnterField(field))
                return false;
        for (auto method : methods_)
            if (!callback.EnterMethod(static_cast<Method *>(method)))
                return false;
        return true;
    }
}