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
namespace DotNetPELib
{
    void Enum::AddValue(Allocator &allocator, std::string Name, longlong Value)
    {
        Type *type = allocator.AllocateType(this);
        Field *field = allocator.AllocateField(Name, type, Qualifiers(Qualifiers::EnumField));
        field->AddEnumValue(Value, size);
        Add(field);
    }
    bool Enum::ILSrcDump(PELib &peLib) const
    {
        ILSrcDumpClassHeader(peLib);
        peLib.Out() << " {" << std::endl;
        DataContainer::ILSrcDump(peLib);
        peLib.Out() << " .field public specialname rtspecialname ";
        Field::ILSrcDumpTypeName(peLib, size);
        peLib.Out() << " value__" << std::endl;
        peLib.Out() << "}" << std::endl;
        return true;
    }
    bool Enum::PEDump(PELib &peLib)
    {
        int peflags = TransferFlags();
        size_t typenameIndex = peLib.PEOut().HashString(Name());
        size_t namespaceIndex = ParentNamespace(peLib);
        size_t extends = peLib.PEOut().EnumBaseClass();
        size_t fieldIndex = peLib.PEOut().NextTableIndex(tField);
        size_t methodIndex = peLib.PEOut().NextTableIndex(tMethodDef);
        TypeDefOrRef extendsClass(TypeDefOrRef::TypeRef, extends);
        DataContainer *parent = Parent();
        if (parent && typeid(*parent) == typeid(Class))
            namespaceIndex = 0;
        TableEntryBase *table = new TypeDefTableEntry(peflags, typenameIndex, namespaceIndex,
            extendsClass, fieldIndex, methodIndex);
        peIndex_ = peLib.PEOut().AddTableEntry(table);

        if (parent && typeid(*parent) == typeid(Class))
        {
            size_t enclosing = ParentClass(peLib);
            table = new NestedClassTableEntry(peIndex_, enclosing);
            peLib.PEOut().AddTableEntry(table);
        }
        DataContainer::PEDump(peLib); // should only be the enumerations
        size_t sz;
        Type::BasicType tsize;
        switch (size)
        {
        case Field::i8:
            tsize = Type::i8;
            break;
        case Field::i16:
            tsize = Type::i16;
            break;
        case Field::i32:
        default:
            tsize = Type::i32;
            break;
        case Field::i64:
            tsize = Type::i64;
            break;
        }
        // add the value member
        Type type(tsize, 0);
        Field field("value__", &type, Qualifiers(0));
        Byte *sig = SignatureGenerator::FieldSig(&field, sz);
        size_t sigindex = peLib.PEOut().HashBlob(sig, sz);
        size_t nameindex = peLib.PEOut().HashString(field.Name());
        table = new FieldTableEntry(FieldTableEntry::Public | FieldTableEntry::SpecialName | FieldTableEntry::RTSpecialName, nameindex, sigindex);
        peIndex_ = peLib.PEOut().AddTableEntry(table);
        delete[] sig;
        return true;
    }
}
