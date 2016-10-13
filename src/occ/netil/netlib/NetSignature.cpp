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
size_t SignatureGenerator::EmbedType(int *buf, int offset, Type *tp)
{
    int rv = 0;
    for (int i=0; i < tp->GetPointer(); i++)
        buf[offset + rv++] = ELEMENT_TYPE_PTR;
    switch (tp->GetBasicType())
    {
        case Type::objectArray:
            buf[offset + rv++] = ELEMENT_TYPE_ARRAY;
            buf[offset + rv++] = 1;
            buf[offset + rv++] = 0;
            //fallthrough
        case Type::object:
            buf[offset+ rv++] = ELEMENT_TYPE_CLASS;
            buf[offset + rv++] = objectBase || (tTypeRef << 24); 
            break;
        case Type::cls:
        {
            Class *cls = (Class *)tp->GetClass();
            if (cls->GetFlags().flags & Qualifiers::Value)
            {
                buf[offset + rv++] = ELEMENT_TYPE_VALUETYPE;
            }
            else
            {
                buf[offset + rv++] = ELEMENT_TYPE_CLASS;
            }
            // for now, it is a typedef.  Later we have to revisit...
            buf[offset +rv++] = cls->GetPEIndex() || (tTypeDef << 24); 
            break;
        }
        case Type::method:
        {
            MethodSignature *sig = tp->GetMethod();
            buf[offset + rv++] = ELEMENT_TYPE_FNPTR;
            rv += CoreMethod(sig, sig->GetParamCount() + sig->GetVarargParamCount(), buf, offset + rv);
            if (sig->GetVarargParamCount())
            {
                workArea[offset + rv++] = ELEMENT_TYPE_SENTINEL;
                for (MethodSignature::viterator it = sig->vbegin(); it != sig->vend(); ++it)
                {
                    rv += EmbedType(workArea, offset + rv, (*it)->GetType());
                }
            }

        }
            break;
        default:
            buf[offset+rv++] = basicTypes[tp->GetBasicType()];
            break;
    }
    return rv;
}
unsigned char *SignatureGenerator::ConvertToBlob(int *buf, int size, int &sz)
{
    sz = 0;
    for (int i=0; i < size; i++)
    {
        if (buf[i] > 0x3fff)
            sz += 4;
        else if (buf[i] > 0x7f)
            sz += 2;
        else 
            sz += 1;
    }
    unsigned char *rv = new unsigned char[sz];
    for (int i=0; i < sz;)
    {
        if (buf[i] > 0x3fff)
        {
            rv[i++] = ((buf[i] >> 24) & 0x1f) | 0xc0;
            rv[i++] = (buf[i] >> 16) &0xff;
            rv[i++] = (buf[i] >> 8) &0xff;
            rv[i++] = buf[i] & 0xff;
        }
        else if (buf[i] > 0x7f)
        {
            rv[i++] = (buf[i] >> 8) | 0x80;
            rv[i++] = buf[i] & 0xff;
        }
        else 
        {
            rv[i++ ] =  buf[i];
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
    if (method->GetFlags() && MethodSignature::Instance)
        flag |= 0x20;
    if (method->GetFlags() && MethodSignature::Vararg && !(method->GetFlags() & MethodSignature::Vararg))
        flag |= 5;
    workArea[size++] = flag;
    workArea[size++] = 0; // generic params
    workArea[size++] = paramCount;
    size += EmbedType(workArea, size, method->GetReturnType());
    for (MethodSignature::iterator it = method->begin(); it != method->end(); ++it)
    {
        size += EmbedType(workArea, size, (*it)->GetType());
    }
    return size - origOffset;
}
unsigned char *SignatureGenerator::MethodDefSig(MethodSignature *method, size_t &sz)
{
    int size = 0;
    size = CoreMethod(method, method->GetParamCount(), workArea, 0);
    return ConvertToBlob(workArea, size, sz);
}
unsigned char *SignatureGenerator::MethodRefSig(MethodSignature *method, size_t &sz)
{
    int size = 0;
    size = CoreMethod(method, method->GetParamCount() + method->GetVarargParamCount(), workArea, 0);
    // variable length args... this is the difference from the methoddef
    if (method->GetVarargParamCount())
    {
        workArea[size++] = ELEMENT_TYPE_SENTINEL;
        for (MethodSignature::viterator it = method->vbegin(); it != method->vend(); ++it)
        {
            size += EmbedType(workArea, size, (*it)->GetType());
        }
    }
    return ConvertToBlob(workArea, size, sz);
}
unsigned char *SignatureGenerator::FieldSig(Field *field, size_t &sz)
{
    int size = 0;
    workArea[size++] = 6; // field sig
    // here we would put the 
    size += EmbedType(workArea, size, field->GetType());
    return ConvertToBlob(workArea, size, sz);
}
unsigned char *SignatureGenerator::LocalVarSig(Method *method, size_t &sz)
{
    int size = 0;
    workArea[size++] = 7; // locals sig
    workArea[size++] = method->size();
    for (Method::iterator it = method->begin(); it != method->end(); ++it)
    {
        size += EmbedType(workArea, size, (*it)->GetType());
    }
    return ConvertToBlob(workArea, size, sz);
}
unsigned char *SignatureGenerator::TypeSig(Type *type, size_t &sz)
{
    int size = 0;
    size += EmbedType(workArea, size, type);
    return ConvertToBlob(workArea, size, sz);
}
} // namespace