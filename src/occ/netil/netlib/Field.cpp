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
#include <iomanip>
namespace DotNetPELib
{
    void Field::AddEnumValue(longlong Value, ValueSize Size)
    {
        if (mode_ == None)
        {
            mode_ = Enum;
            enumValue_ = Value;
            size_ = Size;
        }
    }
    void Field::AddInitializer(Byte *bytes, int len)
    {
        if (mode_ == None)
        {
            mode_ = Bytes;
            byteValue_ = bytes;
            byteLength_ = len;
        }
    }
    bool Field::ILSrcDumpTypeName(PELib &peLib, Field::ValueSize size)
    {
        switch (size)
        {
        case Field::i8:
            peLib.Out() << " int8";
            break;
        case Field::i16:
            peLib.Out() << " int16";
            break;
        case Field::i32:
        default:
            peLib.Out() << " int32";
            break;
        case Field::i64:
            peLib.Out() << " int64";
            break;
        }
        return true;
    }
    bool Field::ILSrcDump(PELib &peLib) const
    {
        peLib.Out() << ".field";
        if ((parent_->Flags().Flags() & Qualifiers::Explicit)
           ||(parent_->Flags().Flags() & Qualifiers::Sequential) && explicitOffset_)
            peLib.Out() << " [" << explicitOffset_ << "]";
        flags_.ILSrcDumpBeforeFlags(peLib);
        flags_.ILSrcDumpAfterFlags(peLib);
        if (FieldType()->GetBasicType() == Type::cls)
        {
            if (FieldType()->GetClass()->Flags().Flags() & Qualifiers::Value)
            {
                peLib.Out() << " valuetype ";
                type_->ILSrcDump(peLib);
            }
            else
            {
                peLib.Out() << " class '";
                type_->ILSrcDump(peLib);
                peLib.Out() << "'";
            }
        }
        else
        {
            peLib.Out() << " ";
            type_->ILSrcDump(peLib);
        }
        peLib.Out() << " '" << name_ << "'";
        switch (mode_)
        {
        case None:
            break;
        case Enum:
            peLib.Out() << " = ";
            ILSrcDumpTypeName(peLib, size_);
            peLib.Out() << "(" << (int)enumValue_ << ")";

            break;
        case Bytes:
            if (byteValue_ && byteLength_)
            {
                peLib.Out() << " at $" << name_ << std::endl;
                peLib.Out() << ".data cil $" << name_ << " = bytearray (" << std::endl << std::hex;
                int i;
                for (i = 0; i < byteLength_; i++)
                {
                    peLib.Out() << std::setw(2) << std::setfill('0') << (int)byteValue_[i] << " ";
                    if (i % 8 == 7 && i != byteLength_ - 1)
                        peLib.Out() << std::endl << "\t";
                }
                peLib.Out() << ")" << std::dec;
            }
            break;
        }
        peLib.Out() << std::endl;
        return true;
    }
    void Field::ObjOut(PELib &peLib, int pass) const
    {
        if (pass == -1) // as a reference, we have to do a full signature because of overloads
                        // and here we need the fully qualified name
        {
            peLib.Out() << std::endl << "$fb" << peLib.FormatName(Qualifiers::GetObjName(name_, parent_));
            peLib.Out() << std::endl << "$fe";
        }
        else
        {
            peLib.Out() << std::endl << "$fb" << peLib.FormatName(name_);
            peLib.Out() << external_ << ",";
            flags_.ObjOut(peLib, pass);
            peLib.Out() << ",";
            type_->ObjOut(peLib, pass);
            peLib.Out() << std::endl << "$fe";
            switch(mode_)
            {
            case None:
                peLib.Out() << std::endl << "$";
                break;
            case Enum:
                peLib.Out() << std::endl << "=";
                peLib.Out() << (longlong)enumValue_ << "," << (int)size_ << ")";
    
                break;
            case Bytes:
                peLib.Out() << std::endl << "(" << std::hex;
                for (int i = 0; i < byteLength_; i++)
                {
                    peLib.Out() << std::setw(2) << std::setfill('0') << (int)byteValue_[i];
                }
                peLib.Out() << ")" << std::dec;
            }
        }
    }
    Field *Field::ObjIn(PELib &peLib, bool definition)
    {
        std::string name = peLib.UnformatName();

        Field *f = nullptr, *rv = nullptr;
        if (definition)
        {
            int external = peLib.ObjInt();
            char ch;
            ch = peLib.ObjChar();
            if (ch != ',')
                peLib.ObjError(oe_syntax);
            Qualifiers flags;
            flags.ObjIn(peLib);
            ch = peLib.ObjChar();
            if (ch != ',')
                peLib.ObjError(oe_syntax);
            Type *type = Type::ObjIn(peLib);
            for (auto field : peLib.GetContainer()->Fields())
            {
                if (field->Name() == name)
                {
                    f = field;
                    break;
                }
            }
            if (!f)
                rv = f = peLib.AllocateField(name, type, flags);
            else if (!f->FieldType()->Matches(type))
                peLib.ObjError(oe_typemismatch);
            if (rv)
                rv->External(external);
            if (peLib.ObjEnd() != 'f')
                peLib.ObjError(oe_syntax);
            Byte *p=nullptr;
            int len = 0, maxlen = 0;
            switch(peLib.ObjChar())
            {
                case '$':
                    break;
                case '=':
                {
                    longlong value = peLib.ObjInt();
                    ch = peLib.ObjChar();
                    if (ch != ',')
                        peLib.ObjError(oe_syntax);
                    ValueSize size = (ValueSize)peLib.ObjInt();
                    if (peLib.ObjChar() != ')')
                        peLib.ObjError(oe_syntax);
                    if (rv)
                        rv->AddEnumValue(value, size);
                    break;
                }
                case '(':
                {
                    int n1, n2;
                    Byte *p = nullptr;
                    int maxLen = 0, len = 0;
                    
                    int val;
                    while ((val = peLib.ObjHex2()) != -1)
                    {
                        if (len >= maxLen)
                        {
                            Byte *p1 = peLib.AllocateBytes(maxlen = maxLen ? maxLen * 2 : 10);
                            memcpy(p1, p, len);
                            p = p1;
                        }
                        p[len++] = val;
                    }
                    if (peLib.ObjChar() != ')')
                        peLib.ObjError(oe_syntax);
                    if (rv)
                        rv->AddInitializer(p, len);
                }
                    break;
                default:
                    peLib.ObjError(oe_syntax);
                    break;
            }
        }
        else
        {
            void *result;
            if (peLib.Find(name,&result) == PELib::s_field)
            {
                rv = f = static_cast<Field *>(result);
            }
            else
            {
                peLib.ObjError(oe_nofield);
            }
            if (peLib.ObjEnd() != 'f')
                peLib.ObjError(oe_syntax);
        }
        return rv;
    }
    bool Field::PEDump(PELib &peLib)
    {
        size_t sz;
        Byte *sig = SignatureGenerator::FieldSig(this, sz);
        size_t sigindex = peLib.PEOut().HashBlob(sig, sz);
        size_t nameindex = peIndex_ = peLib.PEOut().HashString(Name());
        if (InAssemblyRef())
        {
            parent_->PEDump(peLib);
            if (type_->GetBasicType() == Type::cls)
            {
                type_->GetClass()->PEDump(peLib);
            }
            MemberRefParent refParent(MemberRefParent::TypeRef, parent_->PEIndex());
            TableEntryBase *table = new MemberRefTableEntry(refParent, nameindex, sigindex);
            peIndex_ = peLib.PEOut().AddTableEntry(table);
        }
        else
        {
            int peflags = 0;
            if (flags_.Flags() & Qualifiers::Public)
                peflags |= FieldTableEntry::Public;
            else if (flags_.Flags() & Qualifiers::Private)
                peflags |= FieldTableEntry::Private;
    
            if (flags_.Flags() & Qualifiers::Static)
                peflags |= FieldTableEntry::Static;
            if (flags_.Flags() & Qualifiers::Literal)
                peflags |= FieldTableEntry::Literal;
            switch (mode_)
            {
            case Enum:
                peflags |= FieldTableEntry::HasDefault; // in the blob;
                break;
            case Bytes:
                if (byteValue_ && byteLength_)
                {
                    peflags |= FieldTableEntry::HasFieldRVA; // in separate memory
                }
                break;
            }
            TableEntryBase *table = new FieldTableEntry(peflags, nameindex, sigindex);
            peIndex_ = peLib.PEOut().AddTableEntry(table);
            delete[] sig;

            if ((parent_->Flags().Flags() & Qualifiers::Explicit)
               ||(parent_->Flags().Flags() & Qualifiers::Sequential) && explicitOffset_)
            {
                TableEntryBase *table = new FieldLayoutTableEntry(explicitOffset_, peIndex_);
                peLib.PEOut().AddTableEntry(table);
            }    
            Byte buf[8];
            *(longlong *)(buf) = enumValue_;
            int type;
            switch (mode_)
            {
            case Enum:
            {
                switch (size_)
                {
                case Field::i8:
                    sz = 1;
                    type = ELEMENT_TYPE_I1;
                    break;
                case Field::i16:
                    sz = 2;
                    type = ELEMENT_TYPE_I2;
                    break;
                case Field::i32:
                default:
                    sz = 4;
                    type = ELEMENT_TYPE_I4;
                    break;
                case Field::i64:
                    sz = 8;
                    type = ELEMENT_TYPE_I8;
                    break;
                }
                // this is NOT compressed like the sigs are...
                size_t valueIndex = peLib.PEOut().HashBlob(&buf[0], sz);
                Constant constant(Constant::FieldDef, peIndex_);
                table = new ConstantTableEntry(type, constant, valueIndex);
                peLib.PEOut().AddTableEntry(table);
    
            }
            case Bytes:
                if (byteValue_ && byteLength_)
                {
                    size_t valueIndex = peLib.PEOut().RVABytes(byteValue_, byteLength_);
                    table = new FieldRVATableEntry(valueIndex, peIndex_);
                    peLib.PEOut().AddTableEntry(table);
                }
                break;
            }
        }
        return true;
    }
}