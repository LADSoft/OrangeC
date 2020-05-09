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
void Property::SetContainer(DataContainer* parent, bool add)
{
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
void Property::CreateFunctions(PELib& peLib, std::vector<Type*>& indices, bool hasSetter)
{
    bool found = false;
    MethodSignature* prototype;
    std::string getter_name = "get_" + name_;
    if (parent_)
        for (auto m : parent_->Methods())
        {
            if (static_cast<Method*>(m)->Signature()->Name() == getter_name)
            {
                found = true;
                getter_ = static_cast<Method*>(m);
                break;
            }
        }
    if (!getter_)
    {
        prototype = peLib.AllocateMethodSignature(getter_name, MethodSignature::Managed, parent_);
        getter_ = peLib.AllocateMethod(prototype, Qualifiers::Public | (instance_ ? Qualifiers::Instance : Qualifiers::Static));
    }
    if (hasSetter)
    {
        std::string setter_name = "set_" + name_;
        if (parent_)
            for (auto m : parent_->Methods())
            {
                if (static_cast<Method*>(m)->Signature()->Name() == setter_name)
                {
                    found = true;
                    setter_ = static_cast<Method*>(m);
                    break;
                }
            }
        if (!setter_)
        {
            prototype = peLib.AllocateMethodSignature(setter_name, MethodSignature::Managed, parent_);
            setter_ = peLib.AllocateMethod(prototype, Qualifiers::Public | (instance_ ? Qualifiers::Instance : Qualifiers::Static));
        }
    }
    if (!found)
    {
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
}
void Property::CallGet(PELib& peLib, CodeContainer* code)
{
    if (getter_)
    {
        code->AddInstruction(
            peLib.AllocateInstruction(Instruction::i_call, peLib.AllocateOperand(peLib.AllocateMethodName(getter_->Signature()))));
    }
}
void Property::CallSet(PELib& peLib, CodeContainer* code)
{
    if (setter_)
    {
        code->AddInstruction(
            peLib.AllocateInstruction(Instruction::i_call, peLib.AllocateOperand(peLib.AllocateMethodName(setter_->Signature()))));
    }
}
bool Property::ILSrcDump(PELib& peLib) const
{
    peLib.Out() << ".property ";
    if (flags_ & SpecialName)
        peLib.Out() << "specialname ";
    if (instance_)
        peLib.Out() << "instance ";
    type_->ILSrcDump(peLib);
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
void Property::ObjOut(PELib& peLib, int pass) const
{
    peLib.Out() << std::endl << "$Pb" << peLib.FormatName(name_) << instance_ << ",";
    peLib.Out() << flags_ << ",";
    type_->ObjOut(peLib, pass);
    getter_->ObjOut(peLib, -1);
    if (setter_)
    {
        peLib.Out() << std::endl << "$Sb";
        setter_->ObjOut(peLib, -1);
        peLib.Out() << std::endl << "$Se";
    }
    peLib.Out() << std::endl << "$Pe";
}
Property* Property::ObjIn(PELib& peLib)
{
    std::string name = peLib.UnformatName();
    int instance = peLib.ObjInt();
    char ch;
    ch = peLib.ObjChar();
    if (ch != ',')
        peLib.ObjError(oe_syntax);
    int flags = peLib.ObjInt();
    ch = peLib.ObjChar();
    if (ch != ',')
        peLib.ObjError(oe_syntax);
    Property* rv = peLib.AllocateProperty();
    Type* type = Type::ObjIn(peLib);
    if (peLib.ObjBegin() != 'm')
        peLib.ObjError(oe_syntax);
    Method* getter = nullptr;
    Method::ObjIn(peLib, false, &getter);
    Method* setter = nullptr;
    if (peLib.ObjBegin() == 'S')
    {
        if (peLib.ObjBegin() != 'm')
            peLib.ObjError(oe_syntax);
        Method::ObjIn(peLib, false, &setter);
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
    rv->flags_ = flags;
    rv->SetType(type);
    rv->Instance(instance);
    rv->Getter(getter);
    rv->Setter(setter);
    return rv;
}
bool Property::PEDump(PELib& peLib)
{
    size_t propertyIndex = peLib.PEOut().NextTableIndex(tProperty);
    size_t nameIndex = peLib.PEOut().HashString(name_);
    size_t sz;
    Byte* sig = SignatureGenerator::PropertySig(this, sz);
    size_t propertySignature = peLib.PEOut().HashBlob(sig, sz);
    delete[] sig;
    TableEntryBase* table = new PropertyTableEntry(flags_, nameIndex, propertySignature);
    peLib.PEOut().AddTableEntry(table);

    Semantics semantics = Semantics(Semantics::Property, propertyIndex);
    // FIXME : Coverity complains that the following 'new' statements leak memory, however, I think
    // the design is that the related constructors have side effects and the whole point of the new is to invoke those
    // however, this is an awkard design that is hard to maintain and should probably be reworked.

    table = new MethodSemanticsTableEntry(MethodSemanticsTableEntry::Getter, getter_->Signature()->PEIndex(), semantics);
    peLib.PEOut().AddTableEntry(table);
    if (setter_)
    {
        table = new MethodSemanticsTableEntry(MethodSemanticsTableEntry::Setter, setter_->Signature()->PEIndex(), semantics);
        peLib.PEOut().AddTableEntry(table);
    }
    return true;
}
void Property::Load(PELib& lib, AssemblyDef& assembly, PEReader& reader, size_t propIndex, size_t startIndex, size_t startSemantics,
                    size_t endSemantics, std::vector<Method*>& methods)
{
    PropertyTableEntry* entry = static_cast<PropertyTableEntry*>(reader.Table(tProperty)[propIndex - 1]);
    Byte buf[256];
    reader.ReadFromString(buf, sizeof(buf), entry->name_.index_);
    name_ = (char*)buf;
    SignatureGenerator::TypeFromPropertyRef(lib, assembly, reader, this, entry->propertyType_.index_);

    const DNLTable& table = reader.Table(tMethodSemantics);
    for (int i = startSemantics; i < endSemantics && i < table.size(); i++)
    {
        MethodSemanticsTableEntry* entry2 = static_cast<MethodSemanticsTableEntry*>(table[i]);
        if (entry2->association_.index_ == propIndex)
        {
            if (entry2->semantics_ & MethodSemanticsTableEntry::Getter)
            {
                getter_ = methods[entry2->method_.index_ - startIndex];
            }
            else if (entry2->semantics_ & MethodSemanticsTableEntry::Setter)
            {
                setter_ = methods[entry2->method_.index_ - startIndex];
            }
            // otherwise ignore it...
        }
    }
}
}  // namespace DotNetPELib