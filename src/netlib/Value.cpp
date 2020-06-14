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
namespace DotNetPELib
{
bool Value::ILSrcDump(PELib& peLib) const
{
    // used for types
    type_->ILSrcDump(peLib);
    return true;
}
void Value::ObjOut(PELib& peLib, int pass) const { type_->ObjOut(peLib, pass); }
Value* Value::ObjIn(PELib& peLib, bool definition)
{
    switch (peLib.ObjBegin())
    {
        case 'B':
        case 't':
        {
            peLib.ObjBack();
            Type* type = Type::ObjIn(peLib);
            Value* rv = peLib.AllocateValue("", type);
            return rv;
        }
        case 'l':
            return Local::ObjIn(peLib, false);
        case 'p':
            return Param::ObjIn(peLib, false);
        case 'f':
            return FieldName::ObjIn(peLib, false);
        case 's':
            return MethodName::ObjIn(peLib, false);
        default:
            peLib.ObjError(oe_syntax);
            break;
    }
    return nullptr;
}
size_t Value::Render(PELib& peLib, int opcode, int operandType, Byte* result) { return type_->Render(peLib, result); }
bool Local::ILSrcDump(PELib& peLib) const
{
    peLib.Out() << "'" << name_ << "/" << index_ << "'";
    return true;
}
void Local::ObjOut(PELib& peLib, int pass) const
{
    peLib.Out() << std::endl << "$lb" << peLib.FormatName(name_) << index_;
    if (pass != -1)
    {
        GetType()->ObjOut(peLib, pass);
    }
    peLib.Out() << std::endl << "$le";
}
Local* Local::ObjIn(PELib& peLib, bool definition)
{
    std::string name = peLib.UnformatName();
    int index = peLib.ObjInt();
    Type* tp = nullptr;
    bool retry = false;
    try
    {
        // type is optional here
        tp = Type::ObjIn(peLib);
    }
    catch (ObjectError&)
    {
        retry = true;
    }
    if (peLib.ObjEnd(!retry) != 'l')
    {
        peLib.ObjError(oe_syntax);
    }
    Local* rv = peLib.AllocateLocal(name, tp);
    rv->Index(index);
    return rv;
}
size_t Local::Render(PELib& peLib, int opcode, int operandType, Byte* result)
{
    int sz = 0;
    if (operandType == Instruction::o_index1)
    {
        *(Byte*)result = index_;
        sz = 1;
    }
    else if (operandType == Instruction::o_index2)
    {
        *(unsigned short*)result = index_;
        sz = 2;
    }
    return sz;
}
bool Param::ILSrcDump(PELib& peLib) const
{
    peLib.Out() << "'" << name_ << "'";
    return true;
}
void Param::ObjOut(PELib& peLib, int pass) const
{
    peLib.Out() << std::endl << "$pb" << peLib.FormatName(name_) << index_;
    if (pass != -1)
    {
        GetType()->ObjOut(peLib, pass);
    }
    peLib.Out() << std::endl << "$pe";
}
Param* Param::ObjIn(PELib& peLib, bool definition)
{
    std::string name = peLib.UnformatName();
    int index = peLib.ObjInt();
    Type* tp = nullptr;
    if (definition)
    {
        tp = Type::ObjIn(peLib);
    }
    if (peLib.ObjEnd() != 'p')
    {
        peLib.ObjError(oe_syntax);
    }
    Param* rv = peLib.AllocateParam(name, tp);
    rv->Index(index);
    return rv;
}
size_t Param::Render(PELib& peLib, int opcode, int operandType, Byte* result)
{
    int sz = 0;
    if (operandType == Instruction::o_index1)
    {
        *(Byte*)result = index_;
        sz = 1;
    }
    else if (operandType == Instruction::o_index2)
    {
        *(unsigned short*)result = index_;
        sz = 2;
    }
    return sz;
}
bool FieldName::ILSrcDump(PELib& peLib) const
{
    if (field_->FieldType()->GetBasicType() == Type::cls)
    {
        if (field_->FieldType()->GetClass()->Flags().Flags() & Qualifiers::Value)
            peLib.Out() << "valuetype ";
        else
            peLib.Out() << "class ";
    }
    field_->FieldType()->ILSrcDump(peLib);
    peLib.Out() << " ";
    peLib.Out() << Qualifiers::GetName(field_->Name(), field_->GetContainer());
    return true;
}
void FieldName::ObjOut(PELib& peLib, int pass) const { field_->ObjOut(peLib, -1); }
FieldName* FieldName::ObjIn(PELib& peLib, bool definition)
{
    Field* fld = Field::ObjIn(peLib, false);
    FieldName* rv = peLib.AllocateFieldName(fld);
    return rv;
}
size_t FieldName::Render(PELib& peLib, int opcode, int operandType, Byte* result)
{
    if (field_->GetContainer() && field_->GetContainer()->InAssemblyRef())
    {
        field_->PEDump(peLib);
        *(DWord*)result = field_->PEIndex() | (tMemberRef << 24);
    }
    else
    {
        *(DWord*)result = field_->PEIndex() | (tField << 24);
    }
    return 4;
}
MethodName::MethodName(MethodSignature* M) : signature_(M), Value("", nullptr) {}
bool MethodName::ILSrcDump(PELib& peLib) const
{
    signature_->ILSrcDump(peLib, false, false, false);
    return true;
}
void MethodName::ObjOut(PELib& peLib, int pass) const { signature_->ObjOut(peLib, -1); }
MethodName* MethodName::ObjIn(PELib& peLib, bool defintion)
{
    MethodSignature* sig = MethodSignature::ObjIn(peLib, nullptr, false);
    MethodName* rv = peLib.AllocateMethodName(sig);
    return rv;
}
size_t MethodName::Render(PELib& peLib, int opcode, int operandType, Byte* result)
{
    if (opcode == Instruction::i_calli)
    {
        if (signature_->PEIndexType() == 0)
            signature_->PEDump(peLib, true);
        *(DWord*)result = signature_->PEIndexType() | (tStandaloneSig << 24);
    }
    else
    {
        if (signature_->PEIndex() == 0 && signature_->PEIndexCallSite() == 0)
            signature_->PEDump(peLib, false);
        if (signature_->PEIndex())
            *(DWord*)result = signature_->PEIndex() | (tMethodDef << 24);
        else if (signature_->Generic().size())
            *(DWord*)result = signature_->PEIndexCallSite() | (tMethodSpec << 24);
        else
            *(DWord*)result = signature_->PEIndexCallSite() | (tMemberRef << 24);
    }
    return 4;
}
}  // namespace DotNetPELib