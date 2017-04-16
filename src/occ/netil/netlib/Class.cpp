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
#include <climits>
namespace DotNetPELib
{

    bool Class::ILSrcDump(PELib &peLib) const
    {
        ILSrcDumpClassHeader(peLib);
        if (extendsFrom_)
        {
            peLib.Out() << " extends " << Qualifiers::GetName("", extendsFrom_);
        }
        peLib.Out() << " {";
        if (pack_ > 0 || size_ > 0)
        {
            peLib.Out() << std::endl;
            if (pack_ > 0)
                peLib.Out() << " .pack " << pack_;
            if (size_ > 0)
                peLib.Out() << " .size " << size_;
        }
        peLib.Out() << std::endl;
        bool rv = DataContainer::ILSrcDump(peLib);
        peLib.Out() << std::endl;
        for (auto p : properties_)
            rv &= p->ILSrcDump(peLib);
        peLib.Out() << "}" << std::endl;
        return rv;
    }
    int Class::TransferFlags() const
    {
        int peflags = TypeDefTableEntry::Class;
        DataContainer *parent = Parent();
        if (parent && typeid(*parent) == typeid(Class))
        {
            if (flags_.Flags() & Qualifiers::Public)
            {
                peflags |= TypeDefTableEntry::NestedPublic;
            }
            else
            {
                peflags |= TypeDefTableEntry::NestedPrivate;
            }
        }
        else
        {
            if (flags_.Flags() & Qualifiers::Public)
            {
                peflags |= TypeDefTableEntry::Public;
            }
        }
        if (flags_.Flags() & Qualifiers::Sequential)
        {
            peflags |= TypeDefTableEntry::SequentialLayout;
        }
        else if (flags_.Flags() & Qualifiers::Explicit)
        {
            peflags |= TypeDefTableEntry::ExplicitLayout;
        }
        if (flags_.Flags() & Qualifiers::Sealed)
        {
            peflags |= TypeDefTableEntry::Sealed;
        }
        if (flags_.Flags() & Qualifiers::Ansi)
        {
            peflags |= TypeDefTableEntry::AnsiClass;
        }
        return peflags;
    }
    bool Class::PEDump(PELib &peLib)
    {
        if (InAssemblyRef())
        {
            if (!peIndex_)
            {
                if (typeid(*parent_) == typeid(Class))
                {
                    parent_->PEDump(peLib);
                    ResolutionScope resolution(ResolutionScope::TypeRef, parent_->PEIndex());
                    size_t typenameIndex = peLib.PEOut().HashString(Name());
                    TableEntryBase *table = new TypeRefTableEntry(resolution, typenameIndex, 0);
                    peIndex_ = peLib.PEOut().AddTableEntry(table);
                }
                else
                {
                    ResolutionScope resolution(ResolutionScope::AssemblyRef, ParentAssembly(peLib));
                    size_t typenameIndex = peLib.PEOut().HashString(Name());
                    size_t namespaceIndex = ParentNamespace(peLib);
                    TableEntryBase *table = new TypeRefTableEntry(resolution, typenameIndex, namespaceIndex);
                    peIndex_ = peLib.PEOut().AddTableEntry(table);
                }
            }
        }
        else
        {
            int peflags = TransferFlags();
            size_t typenameIndex = peLib.PEOut().HashString(Name());
            size_t namespaceIndex = ParentNamespace(peLib);
            size_t extends = (flags_.Flags() & Qualifiers::Value) ? peLib.PEOut().ValueBaseClass() : peLib.PEOut().ObjectBaseClass();
            size_t fieldIndex = peLib.PEOut().NextTableIndex(tField);
            size_t methodIndex = peLib.PEOut().NextTableIndex(tMethodDef);
            DataContainer *parent = Parent();
            if (extendsFrom_)
            {
                if (!extendsFrom_->PEIndex())
                    extendsFrom_->PEDump(peLib);
                extends = extendsFrom_->PEIndex();
            }
            if (parent && typeid(*parent) == typeid(Class))
                namespaceIndex = 0;
            int typeType = TypeDefOrRef::TypeRef;
            if (extendsFrom_ && !extendsFrom_->InAssemblyRef())
                typeType = TypeDefOrRef::TypeDef;
            TypeDefOrRef extendsClass(typeType, extends);
            TableEntryBase *table = new TypeDefTableEntry(peflags, typenameIndex, namespaceIndex,
                extendsClass, fieldIndex, methodIndex);
            peIndex_ = peLib.PEOut().AddTableEntry(table);
    
            if (pack_ > 0 || size_ > 0)
            {
                int mypack_ = pack_;
                int mysize_ = size_;
                if (mypack_ <= 0)
                    mypack_ = 1;
                if (mysize_ <= 0)
                    mysize_ = 1;
                table = new ClassLayoutTableEntry(mypack_, mysize_, peIndex_);
                peLib.PEOut().AddTableEntry(table);
            }
            if (parent && typeid(*parent) == typeid(Class))
            {
                size_t enclosing = ParentClass(peLib);
                table = new NestedClassTableEntry(peIndex_, enclosing);
                peLib.PEOut().AddTableEntry(table);
            }
            DataContainer::PEDump(peLib);
            if (properties_.size())
            {
                size_t propertyIndex = peLib.PEOut().NextTableIndex(tProperty);
                table = new PropertyMapTableEntry(peIndex_, propertyIndex);
                peLib.PEOut().AddTableEntry(table);
                for (auto p : properties_)
                    p->PEDump(peLib);
            }
        }
        return true;
    }
    void Class::ILSrcDumpClassHeader(PELib &peLib) const
    {
        peLib.Out() << ".class";
        DataContainer *parent = Parent();
        if (parent && typeid(*parent) == typeid(Class))
            peLib.Out() << " nested";
        flags_.ILSrcDumpBeforeFlags(peLib);
        flags_.ILSrcDumpAfterFlags(peLib);
        peLib.Out() << " '" << name_ << "'";
    }
    void Class::ObjOut(PELib &peLib, int pass) const
    {
        if (pass == -1)
        {
            peLib.Out() << std::endl << "$cb" << peLib.FormatName(Qualifiers::GetObjName(name_, parent_)) << "$cb";
            peLib.Out() << std::endl << "$ce" << "$ce";
        }
        else
        {
            peLib.Out() << std::endl << "$cb" << peLib.FormatName(name_) << external_ << ",";
            peLib.Out() << pack_ << "," << size_ << ",";
            flags_.ObjOut(peLib, pass);
            DataContainer::ObjOut(peLib, pass);
            if (pass == 3)
            {
                for (auto p : properties_)
                    p->ObjOut(peLib, pass);
            }
            peLib.Out() << std::endl << "$ce";
        }
    }
    Class *Class::ObjIn(PELib &peLib, bool definition)
    {
        Class *rv = nullptr;
        std::string name = peLib.UnformatName();
        if (definition)
        {
            // here we are doing a definition
            int external = peLib.ObjInt();
            char ch;
            ch = peLib.ObjChar();
            if (ch != ',')
                peLib.ObjError(oe_syntax);
            int pack = peLib.ObjInt();
            ch = peLib.ObjChar();
            if (ch != ',')
                peLib.ObjError(oe_syntax);
            int size = peLib.ObjInt();
            ch = peLib.ObjChar();
            if (ch != ',')
                peLib.ObjError(oe_syntax);
            Qualifiers flags;
            flags.ObjIn(peLib);
            DataContainer *temp;
            Class *c;
            temp = peLib.GetContainer()->FindContainer(name);
            if (temp && typeid(*temp) != typeid(Class))
                peLib.ObjError(oe_noclass);
            if (!temp)
                rv = c = peLib.AllocateClass(name, flags, pack, size);
            else
                c = static_cast<Class *>(temp);
            if (rv)
                rv->External(external);
            ((DataContainer *)c)->ObjIn(peLib);
            peLib.PushContainer(c);
            while (peLib.ObjBegin() == 'P')
                c->Add(Property::ObjIn(peLib));
            peLib.PopContainer();
            if (peLib.ObjEnd(false) != 'c')
                peLib.ObjError(oe_syntax);
        }
        else
        {
            // if we get here it is as an operand
            void *result;
            if (peLib.Find(name,&result) == PELib::s_class)
            {
                rv = static_cast<Class *>(result);
            }
            else
            {
                peLib.ObjError(oe_noclass);
            }
            if (peLib.ObjEnd() != 'c')
                peLib.ObjError(oe_syntax);
        }
        return rv;
    }
    void Class::Load(PELib &lib, AssemblyDef &assembly, PEReader &reader, size_t clsIndex, int startField, int endField, int startMethod, int endMethod, int startSemantics, int endSemantics)
    {
        const DNLTable &table = reader.Table(tField);
        int top = table.size();
        for (int i=startField; i < endField && i <= top; i++)
        {
            Byte buf[256];
            FieldTableEntry *entry = static_cast<FieldTableEntry *>(table[i-1]);
            if ((entry->flags_ & FieldTableEntry::FieldAccessMask) == FieldTableEntry::Public)
            {
                reader.ReadFromString(buf, sizeof(buf), entry->nameIndex_.index_);
                Field *field = lib.AllocateField((char *)buf, nullptr, entry->flags_);
                Add(field);
                SignatureGenerator::TypeFromFieldRef(lib, assembly, reader, field, entry->signatureIndex_.index_);
            }
        }
        std::vector<int> paramIndexes;
        std::vector<MethodSignature *> sigs;
        std::vector<Method *> methods;
        const DNLTable &table1 = reader.Table(tMethodDef);
        int i;
        top = table1.size();
        for (i=startMethod; i < endMethod && i <= top; i++)
        {
            Byte buf[256];
            MethodDefTableEntry *entry = static_cast<MethodDefTableEntry *>(table1[i-1]);
            if ((entry->flags_ & MethodDefTableEntry::MemberAccessMask) == MethodDefTableEntry::Public)
            {
                reader.ReadFromString(buf, sizeof(buf), entry->nameIndex_.index_);
                MethodSignature * sig = 
                    lib.AllocateMethodSignature((char *)buf, 
                                               MethodSignature::Managed, this);
                SignatureGenerator::TypeFromMethodRef(lib, assembly, reader, sig, entry->signatureIndex_.index_);
                Method *method = lib.AllocateMethod(sig, entry->flags_, false);
                Add(method);
                sigs.push_back(sig);
                methods.push_back(method);
            }
            else
            {
                sigs.push_back(nullptr);
                methods.push_back(nullptr);
            }
            paramIndexes.push_back(entry->paramIndex_.index_);
        }
        if (i < table1.size())
        {
            MethodDefTableEntry *entry = static_cast<MethodDefTableEntry *>(table1[i-1]);
            paramIndexes.push_back(entry->paramIndex_.index_);
        }
        else
        {
            paramIndexes.push_back(INT_MAX);
        }
        top = paramIndexes.size() - 1;
        i = 0;
        for (auto pentry : sigs)
        {
            if (i < top && pentry)
                pentry->Load(lib, assembly, reader, paramIndexes[i ] , paramIndexes[i+1]);
            i++;
        }
        const DNLTable &table2 = reader.Table(tPropertyMap);
        PropertyMapTableEntry *entry2;
        int end = table2.size();
        for (int i = 0; i < end; i++)
        {
            entry2 = static_cast<PropertyMapTableEntry *>(table2[i]);
            size_t propIndex = entry2->propertyList_.index_;
            if (entry2->parent_.index_ == clsIndex)
            {
                if (i < end - 1)
                {
                    entry2 = static_cast<PropertyMapTableEntry *>(table2[i + 1]);
                    end = entry2->propertyList_.index_;
                }
                else
                {
                    end = reader.Table(tProperty).size()+1;
                }
                for (int j = propIndex; j < end; j++)
                {
                    Property *prop = lib.AllocateProperty();
                    prop->SetContainer(this, false);
                    properties_.push_back(prop);
                    prop->Load(lib, assembly, reader, j, startMethod, startSemantics, endSemantics, methods);
                }
                break;
            }
        }
    }
    bool Class::Traverse(Callback &callback) const
    {
        if (!DataContainer::Traverse(callback))
            return true;
        for (auto property : properties_)
            if (!callback.EnterProperty(property))
                return false;
        return true;
    }
}