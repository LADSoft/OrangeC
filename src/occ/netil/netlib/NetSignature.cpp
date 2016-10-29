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
    int SignatureGenerator::workArea[400 * 1024];

    int SignatureGenerator::basicTypes[] =
    {
        0,0,ELEMENT_TYPE_VOID,
        ELEMENT_TYPE_I1, ELEMENT_TYPE_U1,
        ELEMENT_TYPE_I2, ELEMENT_TYPE_U2,
        ELEMENT_TYPE_I4, ELEMENT_TYPE_U4,
        ELEMENT_TYPE_I8, ELEMENT_TYPE_U8,
        ELEMENT_TYPE_I, ELEMENT_TYPE_U,
        ELEMENT_TYPE_R4, ELEMENT_TYPE_R8,
        0,0,
        ELEMENT_TYPE_STRING
    };

    int SignatureGenerator::objectBase;

    size_t SignatureGenerator::EmbedType(int *buf, int offset, Type *tp)
    {
        int rv = 0;
        for (int i = 0; i < tp->PointerLevel(); i++)
            buf[offset + rv++] = ELEMENT_TYPE_PTR;
        switch (tp->GetBasicType())
        {
        case Type::objectArray:
            buf[offset + rv++] = ELEMENT_TYPE_SZARRAY;
            // fallthrough
        case Type::object:
            buf[offset + rv++] = ELEMENT_TYPE_OBJECT;
            break;
        case Type::cls:
        {
            Class *cls = (Class *)tp->GetClass();
            if (cls->Flags().Flags() & Qualifiers::Value)
            {
                buf[offset + rv++] = ELEMENT_TYPE_VALUETYPE;
            }
            else
            {
                buf[offset + rv++] = ELEMENT_TYPE_CLASS;
            }
            // for now, it is a typedef.  Later we have to revisit...
            buf[offset + rv++] = (cls->PEIndex() << 2) | TypeDefOrRef::TypeDef;
            break;
        }
        case Type::method:
        {
            MethodSignature *sig = tp->GetMethod();
            buf[offset + rv++] = ELEMENT_TYPE_FNPTR;
            rv += CoreMethod(sig, sig->ParamCount() + sig->VarargParamCount(), buf, offset + rv);
            if (sig->VarargParamCount())
            {
                workArea[offset + rv++] = ELEMENT_TYPE_SENTINEL;
                for (MethodSignature::viterator it = sig->vbegin(); it != sig->vend(); ++it)
                {
                    rv += EmbedType(workArea, offset + rv, (*it)->GetType());
                }
            }

        }
        break;
        case Type::Void:
            if (tp->PEIndex())
            {
                buf[offset + rv++] = ELEMENT_TYPE_CLASS;
                buf[offset + rv++] = (tp->PEIndex() << 2) | TypeDefOrRef::TypeRef;
                break;
            }
            // fall through
        default:
            buf[offset + rv++] = basicTypes[tp->GetBasicType()];
            break;
        }
        return rv;
    }
    Byte *SignatureGenerator::ConvertToBlob(int *buf, int size, size_t &sz)
    {
        sz = 0;
        for (int i = 0; i < size; i++)
        {
            if (buf[i] > 0x3fff)
                sz += 4;
            else if (buf[i] > 0x7f)
                sz += 2;
            else
                sz += 1;
        }
        Byte *rv = new Byte[sz];
        int pos = 0;
        for (int i = 0; i < size; i++)
        {
            if (buf[i] > 0x3fff)
            {
                rv[pos++] = ((buf[i] >> 24) & 0x1f) | 0xc0;
                rv[pos++] = (buf[i] >> 16) & 0xff;
                rv[pos++] = (buf[i] >> 8) & 0xff;
                rv[pos++] = buf[i] & 0xff;
            }
            else if (buf[i] > 0x7f)
            {
                rv[pos++] = (buf[i] >> 8) | 0x80;
                rv[pos++] = buf[i] & 0xff;
            }
            else
            {
                rv[pos++] = buf[i];
            }
        }
        return rv;
    }
    size_t SignatureGenerator::CoreMethod(MethodSignature *method, int paramCount, int *buf, int offset)
    {
        int origOffset = offset;
        int size = offset;
        int flag = 0;
        // for static members, flag will usually remain 0
        if (method->Flags() & MethodSignature::Instance)
            flag |= 0x20;
        if ((method->Flags() & MethodSignature::Vararg) && !(method->Flags() & MethodSignature::Managed))
            flag |= 5;
        workArea[size++] = flag;
        workArea[size++] = paramCount;
        size += EmbedType(workArea, size, method->ReturnType());
        for (auto it = method->begin(); it != method->end(); ++it)
        {
            size += EmbedType(workArea, size, (*it)->GetType());
        }
        return size - origOffset;
    }
    Byte *SignatureGenerator::MethodDefSig(MethodSignature *method, size_t &sz)
    {
        int size = 0;
        size = CoreMethod(method, method->ParamCount(), workArea, 0);
        return ConvertToBlob(workArea, size, sz);
    }
    Byte *SignatureGenerator::MethodRefSig(MethodSignature *method, size_t &sz)
    {
        int size = 0;
        size = CoreMethod(method, method->ParamCount() + method->VarargParamCount(), workArea, 0);
        // variable length args... this is the difference from the methoddef
        if ((method->Flags() & MethodSignature::Vararg) && !(method->Flags() & MethodSignature::Managed))
        {
            workArea[size++] = ELEMENT_TYPE_SENTINEL;
            for (MethodSignature::viterator it = method->vbegin(); it != method->vend(); ++it)
            {
                size += EmbedType(workArea, size, (*it)->GetType());
            }
        }
        return ConvertToBlob(workArea, size, sz);
    }
    Byte *SignatureGenerator::FieldSig(Field *field, size_t &sz)
    {
        int size = 0;
        workArea[size++] = 6; // field sig
        // here we would put the 
        size += EmbedType(workArea, size, field->FieldType());
        return ConvertToBlob(workArea, size, sz);
    }
    Byte *SignatureGenerator::LocalVarSig(Method *method, size_t &sz)
    {
        int size = 0;
        workArea[size++] = 7; // locals sig
        workArea[size++] = method->size();
        for (auto it = method->begin(); it != method->end(); ++it)
        {
            size += EmbedType(workArea, size, (*it)->GetType());
        }
        return ConvertToBlob(workArea, size, sz);
    }
    Byte *SignatureGenerator::TypeSig(Type *type, size_t &sz)
    {
        int size = 0;
        size += EmbedType(workArea, size, type);
        return ConvertToBlob(workArea, size, sz);
    }
} // namespace