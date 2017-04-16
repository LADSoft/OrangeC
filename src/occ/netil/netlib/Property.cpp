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
#include <sstream>

namespace DotNetPELib
{
void Property::Instance(bool instance)
{
    instance_ = instance;
    if (getter_)
        getter_->Instance(instance);
    if (setter_)
        setter_->Instance(instance);
}
void Property::SetContainer(DataContainer *parent, bool add) {
    if (!parent_)
    {
        parent_ = parent;
        if (add)
        {
            parent_->Add(getter_);
            getter_->Signature()->SetContainer(parent);
            if (setter_)
            {
                parent_->Add(setter_);
                setter_->Signature()->SetContainer(parent);
            }
        }
    }
}
void Property::CreateFunctions(PELib &peLib, std::vector<Type *>& indices, bool hasSetter)
{
    std::string getter_name = "get_" + name_;
    MethodSignature *prototype = peLib.AllocateMethodSignature(getter_name, MethodSignature::Managed, parent_);
    getter_ = peLib.AllocateMethod(prototype, Qualifiers::Public | (instance_ ? Qualifiers::Instance : Qualifiers::Static));
    if (hasSetter)
    {
        std::string setter_name = "set_" + name_;
        MethodSignature *prototype = peLib.AllocateMethodSignature(setter_name, MethodSignature::Managed, parent_);
        setter_ = peLib.AllocateMethod(prototype, Qualifiers::Public | (instance_ ? Qualifiers::Instance : Qualifiers::Static));
    }
    int counter = 1;
    for (auto i : indices)
    {
        std::stringstream stream;
        stream << "P" << counter++;
        char cbuf[256];
        stream.getline(cbuf, sizeof(cbuf));
        getter_->Signature()->AddParam(peLib.AllocateParam(cbuf, i));
        if (setter_)
            setter_->Signature()->AddParam(peLib.AllocateParam(cbuf, i));
    }
    getter_->Signature()->ReturnType(type_);
    if (setter_)
    {
        setter_->Signature()->AddParam(peLib.AllocateParam("Value", type_));
        setter_->Signature()->ReturnType(peLib.AllocateType(Type::Void, 0));
    }
}
void Property::CallGet(PELib &peLib, CodeContainer *code)
{
    if (getter_)
    {
        code->AddInstruction(peLib.AllocateInstruction(Instruction::i_call,
            peLib.AllocateOperand(peLib.AllocateMethodName(getter_->Signature()))));
    }
}
void Property::CallSet(PELib &peLib, CodeContainer *code)
{
    if (setter_)
    {
        code->AddInstruction(peLib.AllocateInstruction(Instruction::i_call,
            peLib.AllocateOperand(peLib.AllocateMethodName(setter_->Signature()))));
    }
}
bool Property::ILSrcDump(PELib &peLib) const
{
    peLib.Out() << ".property ";
    if (flags_ & SpecialName)
        peLib.Out() << "specialname ";
    if (instance_)
        peLib.Out() << "instance ";
    type_->ILSrcDump(peLib) ;
    peLib.Out() << " " << name_ << "() {" << std::endl;
    peLib.Out() << ".get ";
    getter_->Signature()->ILSignatureDump(peLib);
    peLib.Out() << std::endl;
    if (setter_)
    {
        peLib.Out() << ".set ";
        setter_->Signature()->ILSignatureDump(peLib);
        peLib.Out() << std::endl;
    }
    peLib.Out() << "}";
    return true;
}
void Property::ObjOut(PELib &peLib, int pass) const
{
    peLib.Out() <<  std::endl << "$Pb" << peLib.FormatName(name_) << instance_ << ",";
    peLib.Out() << flags_ << ",";
    type_->ObjOut(peLib, pass);
    getter_->ObjOut(peLib, pass);
    if (setter_)
    {
        peLib.Out() <<  std::endl << "$Sb";
        setter_->ObjOut(peLib, pass);
        peLib.Out() <<  std::endl << "$Se";
    }
    peLib.Out() <<  std::endl << "$Pe";
}
Property *Property::ObjIn(PELib &peLib)
{
    std::string name =  peLib.UnformatName();
    int instance = peLib.ObjInt();
    char ch;
    ch = peLib.ObjChar();
    if (ch != ',')
        peLib.ObjError(oe_syntax);
    Property *rv = peLib.AllocateProperty();
    Type *type = Type::ObjIn(peLib);
    Method *getter = Method::ObjIn(peLib);
    Method *setter = nullptr;
    if (peLib.ObjBegin() == 'S')
    {
        setter = Method::ObjIn(peLib);
        if (peLib.ObjEnd() != 'S')
            peLib.ObjError(oe_syntax);
        if (peLib.ObjEnd() != 'P')
            peLib.ObjError(oe_syntax);
    }
    else if (peLib.ObjEnd(false) != 'P')
    {
        peLib.ObjError(oe_syntax);
    }
    rv->Name(name);
    rv->SetType(type);
    rv->Instance(instance);
    rv->Getter(getter);
    rv->Setter(setter);
    return rv;
}
bool Property::PEDump(PELib &peLib)
{
    size_t propertyIndex = peLib.PEOut().NextTableIndex(tProperty);
    size_t nameIndex = peLib.PEOut().HashString(name_);
    size_t sz;
    Byte *sig = SignatureGenerator::PropertySig(this, sz);
    size_t propertySignature = peLib.PEOut().HashBlob(sig, sz);
    delete[] sig;
    TableEntryBase *table = new PropertyTableEntry(flags_, nameIndex, propertySignature);
    peLib.PEOut().AddTableEntry(table);

    Semantics semantics = Semantics(Semantics::Property, propertyIndex);
    table = new MethodSemanticsTableEntry(MethodSemanticsTableEntry::Getter, getter_->Signature()->PEIndex(), semantics );

    if (setter_)
    {
        table = new MethodSemanticsTableEntry(MethodSemanticsTableEntry::Setter, setter_->Signature()->PEIndex(), semantics);
    }
    return true;
}
void Property::Load(PELib &lib, AssemblyDef &assembly, PEReader &reader, size_t propIndex, size_t startIndex, size_t startSemantics, size_t endSemantics, std::vector<Method *>& methods)
{
    PropertyTableEntry *entry = static_cast<PropertyTableEntry *>(reader.Table(tProperty)[propIndex-1]);
    Byte buf[256];
    reader.ReadFromString(buf, sizeof(buf), entry->name_.index_);
    name_ = (char *)buf;
    SignatureGenerator::TypeFromPropertyRef(lib, assembly, reader, this, entry->propertyType_.index_);

    const DNLTable & table = reader.Table(tMethodSemantics);
    for (int i = startSemantics; i < endSemantics && i < table.size(); i++)
    {
        MethodSemanticsTableEntry *entry2 = static_cast<MethodSemanticsTableEntry *>(table[i]);
        if (entry2->association_.index_ == propIndex)
        {
            if (entry2->semantics_ == MethodSemanticsTableEntry::Getter)
            {
                getter_ = methods[entry2->method_.index_ - startIndex];
            }
            else if (entry2->semantics_ == MethodSemanticsTableEntry::Setter)
            {
                setter_ = methods[entry2->method_.index_ - startIndex];
            }
            // otherwise ignore it...
        }
    }
}
}