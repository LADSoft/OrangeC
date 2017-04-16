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
#include <stdio.h>
namespace DotNetPELib
{

    char *Type::typeNames_[] = {
        "", "", "void", "bool", "char", "int8", "uint8", "int16", "uint16", "int32", "uint32", "int64", "uint64", "int", "uint",
        "float32", "float64", "object", "string"
    };
    char *BoxedType::typeNames_[] = { "", "", "", "Bool", "Char", "Int8", "UInt8",
        "Int16", "UInt16", "Int32", "UInt32",
        "Int64", "UInt64", "Int", "UInt", "Single", "Double", "", "String"
    };
    bool Type::Matches(Type *right) const
    {
        if (tp_ != right->tp_)
            return false;
        if (arrayLevel_ != right->arrayLevel_)
            return false;
        if (pointerLevel_ != right->pointerLevel_)
            return false;
        if (byRef_ != right->byRef_)
            return false;
        if (tp_ == cls && typeRef_ != right->typeRef_)
            return false;
        if (tp_ == method && methodRef_ != right->methodRef_)
            return false;
        return true;
    }
    bool Type::ILSrcDump(PELib &peLib) const
    {
        if (tp_ == cls)
        {
            if (typeRef_->Flags().Flags() & Qualifiers::Value)
                peLib.Out() << "'" << Qualifiers::GetName("", typeRef_, true) << "'";
            else
                peLib.Out() << Qualifiers::GetName("", typeRef_, true);
        }
        else if (tp_ == method)
        {
            peLib.Out() << "method ";
            methodRef_->ILSrcDump(peLib, false, true, true);
        }
        else
        {
            peLib.Out() << typeNames_[tp_];
        }
        if (arrayLevel_ == 1)
        {
            peLib.Out() << " []";
        }
        else if (arrayLevel_)
        {
            peLib.Out() << " [";
            for (int i = 0; i < arrayLevel_; i++)
            {
                if (i != 0)
                    peLib.Out() << ", 0...";
                else
                    peLib.Out() << "0...";
            }
            peLib.Out() << "]";
        }
        for (int i = 0; i < pointerLevel_; i++)
            peLib.Out() << " *";
        if (byRef_)
            peLib.Out() << "&";
        return true;
    }
    void Type::ObjOut(PELib &peLib, int pass) const
    {
        peLib.Out() <<  std::endl << "$tb" << tp_ << "," << byRef_ << "," << arrayLevel_ << "," << pointerLevel_;
        if (tp_ == cls)
        {
            typeRef_->ObjOut(peLib, -1);
        }
        else if (tp_ == method)
        {
            methodRef_->ObjOut(peLib, -1);
        }
        peLib.Out() <<  std::endl << "$te";
    }
    Type *Type::ObjIn(PELib &peLib)
    {
        if (peLib.ObjBegin() == 'B')
        {
            Type *rv = BoxedType::ObjIn(peLib);
            if (peLib.ObjEnd() != 'B')
                peLib.ObjError(oe_syntax);
        }
        else if (peLib.ObjBegin(false) == 't')
        {

            BasicType tp = (BasicType)peLib.ObjInt();
            char ch;
            ch = peLib.ObjChar();
            if (ch != ',')
                peLib.ObjError(oe_syntax);
            int byRef = peLib.ObjInt();
            ch = peLib.ObjChar();
            if (ch != ',')
                peLib.ObjError(oe_syntax);
            int arrayLevel = peLib.ObjInt();
            ch = peLib.ObjChar();
            if (ch != ',')
                peLib.ObjError(oe_syntax);
            int pointerLevel = peLib.ObjInt();
            Type *rv = nullptr;
            if (tp == cls)
            {
                DataContainer *typeref = nullptr;
                if (peLib.ObjBegin() == 'c')
                {
                    typeref = Class::ObjIn(peLib, false);
                }
                else if (peLib.ObjBegin(false) == 'E')
                {
                    typeref = Enum::ObjIn(peLib, false);
                }
                else
                {
                    peLib.ObjError(oe_syntax);
                }
                rv = peLib.AllocateType(typeref);
            }
            else if (tp == method)
            {
                MethodSignature *methodRef = MethodSignature::ObjIn(peLib, nullptr);
                rv = peLib.AllocateType(methodRef);
            }
            else
            {
                rv = peLib.AllocateType(tp, 0);
            }
            rv->PointerLevel(pointerLevel);
            rv->ArrayLevel(arrayLevel);
            rv->ByRef(byRef);
            if (peLib.ObjEnd() != 't')
                peLib.ObjError(oe_syntax);
            return rv;
        }
        else
        {
            peLib.ObjError(oe_syntax);
        }
        return nullptr;
    }
    size_t Type::Render(PELib &peLib, Byte *result)
    {
        switch (tp_)
        {
        case cls:
            if (typeRef_->InAssemblyRef())
            {
                typeRef_->PEDump(peLib);
                *(int *)result = typeRef_->PEIndex() | (tTypeRef << 24);
            }
            else
            {
                *(int *)result = typeRef_->PEIndex() | (tTypeDef << 24);
            }
            return 4;
            break;
        case method:
        default:
        {
            if (!peIndex_)
            {
                // if rendering a method as a type we are always going to put the sig
                // in the type spec table
                size_t sz;
                Byte *sig = SignatureGenerator::TypeSig(this, sz);
                size_t signature = peLib.PEOut().HashBlob(sig, sz);
                delete[] sig;
                TypeSpecTableEntry *table = new TypeSpecTableEntry(signature);
                peIndex_ = peLib.PEOut().AddTableEntry(table);
            }
            *(int *)result = peIndex_ | (tTypeSpec << 24);
            return 4;
        }
        break;
        }
        return true;
    }
    bool BoxedType::ILSrcDump(PELib &peLib) const
    {
        // no point in looking up the type name in the assembly for this...
        peLib.Out() << "[mscorlib]System." << typeNames_[tp_];
        return true;
    }
    void BoxedType::ObjOut(PELib &peLib, int pass) const
    {
        peLib.Out() <<  std::endl << "$Bb" << tp_ <<  std::endl << "$Be";
    }
    BoxedType *BoxedType::ObjIn(PELib &peLib)
    {
        Type::BasicType type = (Type::BasicType)peLib.ObjInt();
        return peLib.AllocateBoxedType(type);
    }
    size_t BoxedType::Render(PELib &peLib, Byte *result)
    {
        if (!peIndex_)
        {
            size_t system = peLib.PEOut().SystemName();
            size_t name = peLib.PEOut().HashString(typeNames_[tp_]);
            AssemblyDef *assembly = peLib.MSCorLibAssembly();
            void *result = nullptr;
            peLib.Find(std::string("System.") + typeNames_[tp_], &result, assembly);
            if (result)
            {
                static_cast<Class *>(result)->PEDump(peLib);
                peIndex_ =  static_cast<Class *>(result)->PEIndex();
            }
        }
        *(int *)result = peIndex_ | (tTypeRef << 24);
        return 4;
    }
}
