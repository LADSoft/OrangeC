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

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#include "DotNetPELib.h"
#include "PEFile.h"
#include <typeinfo>
namespace DotNetPELib
{

    bool Class::ILSrcDump(PELib &peLib)
    {
        ILSrcDumpClassHeader(peLib);
        peLib.Out() << " {";
        if (pack > 0 || size > 0)
        {
            peLib.Out() << std::endl;
            if (pack > 0)
                peLib.Out() << " .pack " << pack;
            if (size > 0)
                peLib.Out() << " .size " << size;
            peLib.Out() << std::endl;
        }
        bool rv = DataContainer::ILSrcDump(peLib);
        peLib.Out() << "}" << std::endl;
        return rv;
    }
    int Class::TransferFlags()
    {
        int peflags = TypeDefTableEntry::Class;
        if (flags.flags & Qualifiers::Nested)
        {
            if (flags.flags & Qualifiers::Public)
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
            if (flags.flags & Qualifiers::Public)
            {
                peflags |= TypeDefTableEntry::Public;
            }
        }
        if (flags.flags & Qualifiers::Sequential)
        {
            peflags |= TypeDefTableEntry::SequentialLayout;
        }
        else if (flags.flags & Qualifiers::Explicit)
        {
            peflags |= TypeDefTableEntry::ExplicitLayout;
        }
        if (flags.flags & Qualifiers::Sealed)
        {
            peflags |= TypeDefTableEntry::Sealed;
        }
        if (flags.flags & Qualifiers::Ansi)
        {
            peflags |= TypeDefTableEntry::AnsiClass;
        }
        return peflags;
    }
    bool Class::PEDump(PELib &peLib)
    {
        int peflags = TransferFlags();
        size_t typenameIndex = peLib.PEOut().HashString(GetName());
        size_t namespaceIndex = GetParentNamespace();
        size_t extends = (flags.flags & Qualifiers::Value) ? peLib.PEOut().ValueBaseClass() : peLib.PEOut().ObjectBaseClass();
        size_t fieldIndex = peLib.PEOut().GetIndex(tField);
        size_t methodIndex = peLib.PEOut().GetIndex(tMethodDef);
        TypeDefOrRef extendsClass(TypeDefOrRef::TypeDef, extends);
        TableEntryBase *table = new TypeDefTableEntry(peflags, typenameIndex, namespaceIndex,
                                                     extendsClass, fieldIndex, methodIndex);
        peIndex = peLib.PEOut().AddTableEntry(table);

        if (pack > 0 || size > 0)
        {
            int mypack =pack;
            int mysize = size;
            if (mypack <=0)
                mypack = 1;
            if (mysize <=0 )
                mysize = 1;
            table = new ClassLayoutTableEntry(mypack, mysize, peIndex);
            peLib.PEOut().AddTableEntry(table);
        }
        if (flags.flags & Qualifiers::Nested)
        {
            size_t enclosing = GetParentClass();
            table = new NestedClassTableEntry(peIndex, enclosing);
            peLib.PEOut().AddTableEntry(table);
        }
        DataContainer::PEDump(peLib);
        return true;
    }
    void Class::ILSrcDumpClassHeader(PELib &peLib)
    {
        peLib.Out() << ".class";
        flags.ILSrcDumpBeforeFlags(peLib);
        flags.ILSrcDumpAfterFlags(peLib);
        peLib.Out() << " '" << name << "'";
    }
}