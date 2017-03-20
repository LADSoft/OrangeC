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
    bool Value::ILSrcDump(PELib &peLib) const
    {
        // used for types
        type_->ILSrcDump(peLib);
        return true;
    }
    size_t Value::Render(PELib &peLib, int opcode, int operandType, Byte *result)
    {
        return type_->Render(peLib, result);
    }
    bool Local::ILSrcDump(PELib &peLib) const
    {
        peLib.Out() << "'" << name_ << "/" << index_ << "'";
        return true;
    }
    size_t Local::Render(PELib &peLib, int opcode, int operandType, Byte *result)
    {
        int sz = 0;
        if (operandType == Instruction::o_index1)
        {
            *(Byte *)result = index_;
            sz = 1;
        }
        else if (operandType == Instruction::o_index2)
        {
            *(unsigned short *)result = index_;
            sz = 2;
        }
        return sz;
    }
    bool Param::ILSrcDump(PELib &peLib) const
    {
        peLib.Out() << "'" << name_ << "'";
        return true;
    }
    size_t Param::Render(PELib &peLib, int opcode, int operandType, Byte *result)
    {
        int sz = 0;
        if (operandType == Instruction::o_index1)
        {
            *(Byte *)result = index_;
            sz = 1;
        }
        else if (operandType == Instruction::o_index2)
        {
            *(unsigned short *)result = index_;
            sz = 2;
        }
        return sz;
    }
    bool FieldName::ILSrcDump(PELib &peLib) const
    {
        if (field_->FieldType()->GetBasicType() == Type::cls)
            if (field_->FieldType()->GetClass()->Flags().Flags() & Qualifiers::Value)
                peLib.Out() << "valuetype ";
            else
                peLib.Out() << "class ";
        field_->FieldType()->ILSrcDump(peLib);
        peLib.Out() << " ";
        peLib.Out() << Qualifiers::GetName(field_->Name(), field_->GetContainer());
        return true;
    }
    size_t FieldName::Render(PELib &peLib, int opcode, int operandType, Byte *result)
    {
        if (field_->GetContainer() && field_->GetContainer()->InAssemblyRef())
        {
            field_->PEDump(peLib);
            *(DWord *)result = field_->PEIndex() | (tMemberRef << 24);
        }
        else
        {
            *(DWord *)result = field_->PEIndex() | (tField << 24);
        }
        return 4;
    }
    MethodName::MethodName(MethodSignature *M) : signature_(M), Value("", nullptr)
    {
    }
    bool MethodName::ILSrcDump(PELib &peLib) const
    {
        signature_->ILSrcDump(peLib, false, false, false);
        return true;
    }
    size_t MethodName::Render(PELib &peLib, int opcode, int operandType, Byte *result)
    {
        if (opcode == Instruction::i_calli)
        {
            if (signature_->PEIndexType() == 0)
                signature_->PEDump(peLib, true);
            *(DWord *)result = signature_->PEIndexType() | (tStandaloneSig << 24);
        }
        else
        {
            if (signature_->PEIndex() == 0 && signature_->PEIndexCallSite() == 0)
                signature_->PEDump(peLib, false);
            if (signature_->PEIndex())
                *(DWord *)result = signature_->PEIndex() | (tMethodDef << 24);
            else
                *(DWord *)result = signature_->PEIndexCallSite() | (tMemberRef << 24);
        }
        return 4;
    }
}