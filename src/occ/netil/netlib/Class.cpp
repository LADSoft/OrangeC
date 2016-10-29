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

    bool Class::ILSrcDump(PELib &peLib) const
    {
        ILSrcDumpClassHeader(peLib);
        peLib.Out() << " {";
        if (pack_ > 0 || size_ > 0)
        {
            peLib.Out() << std::endl;
            if (pack_ > 0)
                peLib.Out() << " .pack " << pack_;
            if (size_ > 0)
                peLib.Out() << " .size " << size_;
            peLib.Out() << std::endl;
        }
        bool rv = DataContainer::ILSrcDump(peLib);
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
        int peflags = TransferFlags();
        size_t typenameIndex = peLib.PEOut().HashString(Name());
        size_t namespaceIndex = ParentNamespace();
        size_t extends = (flags_.Flags() & Qualifiers::Value) ? peLib.PEOut().ValueBaseClass() : peLib.PEOut().ObjectBaseClass();
        size_t fieldIndex = peLib.PEOut().NextTableIndex(tField);
        size_t methodIndex = peLib.PEOut().NextTableIndex(tMethodDef);
        DataContainer *parent = Parent();
        if (parent && typeid(*parent) == typeid(Class))
            namespaceIndex = 0;
        TypeDefOrRef extendsClass(TypeDefOrRef::TypeRef, extends);
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
            size_t enclosing = ParentClass();
            table = new NestedClassTableEntry(peIndex_, enclosing);
            peLib.PEOut().AddTableEntry(table);
        }
        DataContainer::PEDump(peLib);

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
}