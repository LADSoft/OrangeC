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
        ELEMENT_TYPE_bool, ELEMENT_TYPE_CHAR,
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
            if (cls->InAssemblyRef())
            {
                buf[offset + rv++] = (cls->PEIndex() << 2) | TypeDefOrRef::TypeRef;
            }
            else
            {
                buf[offset + rv++] = (cls->PEIndex() << 2) | TypeDefOrRef::TypeDef;
            }
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
    size_t SignatureGenerator::LoadIndex(Byte *buf, size_t &start, size_t &len)
    {
        if (!(buf[start] & 0x80))
        {
            len--;
            return buf[start++];
        }
        else if (!(buf[start] & 0x40))
        {

            size_t rv = ((buf[start+0] & 0x7f) << 8) + buf[start+1];
            start+=2 ;
            len-=2;
            return rv;
        }
        else
        {
            size_t rv = ((buf[start] & 0x3f) << 24) + (buf[start+1] << 16) + (buf[start+2] << 8) + buf[start+3];
            start+=4 ;
            len-=4;
            return rv;
        }
    }
    std::string SignatureGenerator::LoadClassName(PEReader &reader, size_t index, AssemblyRefTableEntry **assembly, bool isDef)
    {
        if (isDef)
        {
            std::string rv;
            bool done = false;
            const DNLTable &table = reader.Table(tNestedClass);
            const DNLTable &table1 = reader.Table(tTypeDef);
            while (!done)
            {
                done = true;
                TypeDefTableEntry *entry = static_cast<TypeDefTableEntry *>(table1[index-1]);
                Byte buf[256];
                reader.ReadFromString(buf, sizeof(buf), entry->typeNameIndex_.index_);
                if (rv.size())
                    rv = std::string(".") + rv;
                rv = std::string((char *)buf) + rv;
                for (auto tentry : table)
                {
                    NestedClassTableEntry *entry = static_cast<NestedClassTableEntry *>(tentry);
                    if (entry->nestedIndex_.index_ == index)
                    {
                        index = entry->enclosingIndex_.index_;
                        done = false;
                        break;
                    }
                }
            }
            return rv;
        }
        else
        {
            std::string rv;
            TypeRefTableEntry *entry = static_cast<TypeRefTableEntry *>(reader.Table(tTypeRef)[index-1]);
            while (entry && entry->resolution_.tag_ == ResolutionScope::TypeRef)
            {
                Byte buf[256];
                reader.ReadFromString(buf, sizeof(buf), entry->typeNameIndex_.index_);
                if (rv.size())
                    rv = std::string(".") + rv;
                rv = std::string((char *)buf) + rv;
                entry = static_cast<TypeRefTableEntry *>(reader.Table(tTypeRef)[entry->resolution_.index_ - 1]);
            }
            if (entry && entry->resolution_.tag_ == ResolutionScope::AssemblyRef)
            {
                Byte buf[256];
                reader.ReadFromString(buf, sizeof(buf), entry->typeNameIndex_.index_);
                if (rv.size())
                    rv = std::string(".") + rv;
                rv = std::string((char *)buf) + rv;
                if (assembly)
                    *assembly = static_cast<AssemblyRefTableEntry *>(reader.Table(tAssemblyRef)[ entry->resolution_.index_-1]);
            }
            return rv;
        }
    }
    std::string SignatureGenerator::LoadNameSpaceName(PEReader &reader, size_t index, bool isDef)
    {
        if (isDef)
        {
            bool done = false;
            const DNLTable &table = reader.Table(tNestedClass);
            while (!done)
            {
                done = true;
                for (auto tentry : table)
                {
                    NestedClassTableEntry *entry = static_cast<NestedClassTableEntry *>(tentry);
                    if (entry->nestedIndex_.index_ == index)
                    {
                        index = entry->enclosingIndex_.index_;
                        done = false;
                        break;
                    }
                }
            }
            TypeDefTableEntry *entry = static_cast<TypeDefTableEntry *>(reader.Table(tTypeDef)[index-1]);
            Byte buf[256];
            reader.ReadFromString(buf, sizeof(buf), entry->typeNameSpaceIndex_.index_);
            return (char *)buf;
        }
        else
        {
            TypeRefTableEntry *entry = static_cast<TypeRefTableEntry *>(reader.Table(tTypeRef)[index-1]);
            while (entry->resolution_.tag_ == ResolutionScope::TypeRef)
            {
                entry = static_cast<TypeRefTableEntry *>(reader.Table(tTypeRef)[entry->resolution_.index_-1]);
            }
            Byte buf[256];
            reader.ReadFromString(buf, sizeof(buf), entry->typeNameSpaceIndex_.index_);
            return (char *)buf;            
        }
    }
    Type *SignatureGenerator::TypeFromTypeRef(PELib& lib, AssemblyDef &assembly, PEReader &reader, size_t index, int pointerLevel)
    {
        Type *rv = nullptr;
        int tag = index & 3;
        index >>= 2;
        if (tag == TypeDefOrRef::TypeDef)
        {
            // in current assembly
            std::string name = LoadClassName(reader, index, nullptr, true);
            std::string nameSpace = LoadNameSpaceName(reader, index, true);
            Class *cls = assembly.LookupClass(lib, nameSpace, name);
            rv = lib.AllocateType(cls);
            rv->PointerLevel(pointerLevel);
            return rv;
        }
        else if (tag == TypeDefOrRef::TypeRef)
        {
            // typeref from another assembly, if the assembly exists look it up
            // otherwise create the assembly and insert the reference.
            AssemblyRefTableEntry *assemblyRef = NULL;
            std::string name = LoadClassName(reader, index, &assemblyRef, false);
            std::string nameSpace = LoadNameSpaceName(reader, index, false);
            Class *cls;
            if (assemblyRef)
            {
                Byte buf[256];
                reader.ReadFromString(buf, sizeof(buf), assemblyRef->nameIndex_.index_);
                cls = lib.LookupClass(reader, (char *)buf, assemblyRef->major_, 
                                     assemblyRef->minor_, assemblyRef->build_, 
                                     assemblyRef->revision_,  
                                     assemblyRef->publicKeyIndex_.index_, nameSpace, name);
            }
            else
            {
                cls = lib.LookupClass(reader, "$$unknown", 0, 0, 0, 0, 0, nameSpace, name);
            }
            rv = lib.AllocateType(cls);
            rv->PointerLevel(pointerLevel);
            return rv;
        }
        else if (tag == TypeDefOrRef::TypeSpec)
        {
            printf("%s\n", "type from type spec");
        }
        if (rv)
            rv->PointerLevel(pointerLevel);
        return rv;
    }
    Type *SignatureGenerator::BasicType(PELib& lib, int typeIndex, int pointerLevel)
    {
        Type::BasicType type = Type::Void;
        switch(typeIndex)
        {
            case ELEMENT_TYPE_VAR:
            case ELEMENT_TYPE_GENERICINST:
            case ELEMENT_TYPE_END:
                return nullptr;
            case ELEMENT_TYPE_VOID:
                type = Type::Void;
                break;
            case ELEMENT_TYPE_bool:
                type = Type::Bool;
                break;
            case ELEMENT_TYPE_CHAR:
                type = Type::Char;
                break;
            case ELEMENT_TYPE_I1:
                type = Type::i8;
                break;
            case ELEMENT_TYPE_U1:
                type = Type::u8;
                break;
            case ELEMENT_TYPE_I2:
                type = Type::i16;
                break;
            case ELEMENT_TYPE_U2:
                type = Type::u16;
                break;
            case ELEMENT_TYPE_I4:
                type = Type::i32;
                break;
            case ELEMENT_TYPE_U4:
                type = Type::u32;
                break;
            case ELEMENT_TYPE_I8:
                type = Type::i64;
                break;
            case ELEMENT_TYPE_U8:
                type = Type::u64;
                break;
            case ELEMENT_TYPE_I:
                type = Type::inative;
                break;
            case ELEMENT_TYPE_U:
                type = Type::unative;
                break;
            case ELEMENT_TYPE_R4:
                type = Type::r32;
                break;
            case ELEMENT_TYPE_R8:
                type = Type::r64;
                break;
            case ELEMENT_TYPE_STRING:
                type = Type::string;
                break;
            case ELEMENT_TYPE_OBJECT:
                type = Type::object;
                break;
            default:
                printf("Unknown type %x\n", typeIndex);
                break;
        }
        return lib.AllocateType(type, pointerLevel);
    }
    void SignatureGenerator::TypeFromMethod(PELib& lib, AssemblyDef &assembly, PEReader &reader, MethodSignature *method, Byte *data, size_t &start, size_t &len)
    {
        if (len)
        {
            int flag = data[start++];
            len--;
            if (flag & 0x10)
            {
                // has generic params, we don't process any further;
                return;
            }
            if (flag & 0x20)
                method->Instance(true);
            int paramCount = data[start++];
            len--;
            method->ReturnType(GetType(lib, assembly, reader, data, start, len));
            for (int i=0; i < paramCount; i++)
            {
                // ignoring custom mods and constraints, for now
                Type *type = GetType(lib, assembly, reader, data, start, len);
                Param *param = lib.AllocateParam("",type);
                method->AddParam(param);
                if (!type)
                    break;
            }
        }
    }
    Type *SignatureGenerator::GetType(PELib& lib, AssemblyDef &assembly, PEReader &reader, Byte *data, size_t &start, size_t &len)
    {
        Type *rv = NULL;
        int pointerLevel = 0;
        while (data[start] == ELEMENT_TYPE_PTR)
        {
            pointerLevel++;
            start++;
            len--;
        }
        len--;
        switch(data[start])
        {
            size_t index;
            MethodSignature *sig;
            case ELEMENT_TYPE_BYREF:
                start++, len--;
                rv = GetType(lib, assembly, reader, data, start, len);
                if (rv)
                    rv->ByRef(true);
                break;
            case ELEMENT_TYPE_TYPEDBYREF: // not supporting this right now...
                rv = nullptr;
                break;
            case ELEMENT_TYPE_SZARRAY:
                start++;
                len--;
                if (data[start] != ELEMENT_TYPE_OBJECT)
                {
                    // error condition, we are going to convert to object but meanwhile
                    // get the type.
                    if (!GetType(lib, assembly, reader, data, start, len))
                        return nullptr;
                }
                else
                {
                    start++;
                    len--;
                }
                rv = lib.AllocateType(Type::objectArray, pointerLevel);
                break;
            case ELEMENT_TYPE_OBJECT:
                start++, len--;
                rv = BasicType(lib, ELEMENT_TYPE_OBJECT, pointerLevel);
                break;
            case ELEMENT_TYPE_VALUETYPE:
            case ELEMENT_TYPE_CLASS:
                start++, len--;
                index = LoadIndex(data, start, len);
                rv = TypeFromTypeRef(lib, assembly, reader, index, pointerLevel);
                break;
            case ELEMENT_TYPE_FNPTR:
                start++, len--;
                sig = lib.AllocateMethodSignature("$$unknown", MethodSignature::Managed, nullptr);
                TypeFromMethod(lib, assembly, reader, sig, data, start, len);
                rv = lib.AllocateType(sig);
                rv->PointerLevel(pointerLevel);
                break;
            default:
                rv = BasicType(lib, data[start], pointerLevel);
                start++, len--;
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
        if (method->Flags() & MethodSignature::InstanceFlag)
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
    Byte *SignatureGenerator::PropertySig(Property *property, size_t &sz)
    {
        int size = 0;
        // a property sig is a modification of the methoddef of the getter
        workArea[size++] = 8;

        size = CoreMethod(property->Getter()->Signature(), property->Getter()->Signature()->ParamCount(), 
            workArea, 0);
        // set the instance flag based on whether the property is a static or nonstatic member
        if (property->Instance())
        {
            workArea[1] |= 0x20;
        }
        else
        {
            workArea[1] &= ~0x20;
        }
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

    void SignatureGenerator::TypeFromMethodRef(PELib& lib, AssemblyDef &assembly, PEReader &reader, MethodSignature *method, size_t blobIndex)
    {
        Byte buf[10000];
        size_t start = 0;
        size_t n = reader.ReadFromBlob(buf, sizeof(buf), blobIndex);
        TypeFromMethod(lib, assembly, reader, method, buf, start, n);
    }
    void SignatureGenerator::TypeFromPropertyRef(PELib& lib, AssemblyDef &assembly, PEReader &reader, Property *property, size_t blobIndex)
    {
        Byte data[10000];
        size_t start = 0;
        size_t len = reader.ReadFromBlob(data, sizeof(data), blobIndex);
        if (len)
        {
            int flag = data[start++];
            len--;
            if (flag & 0x20)
                property->Instance(true);
            else
                property->Instance(false);
            int paramCount = data[start++];
            len--;
            property->SetType(GetType(lib, assembly, reader, data, start, len));
            // the rest of the method def signature for the getter is here HOWEVER
            // we don't care beyond the getter's type for these purposes so just ignore it.
        }
    }
    void SignatureGenerator::TypeFromFieldRef(PELib& lib, AssemblyDef &assembly, PEReader &reader, Field *field, size_t blobIndex)
    {
        Byte buf[256];
        size_t n = reader.ReadFromBlob(buf, sizeof(buf), blobIndex);
        if (n)
        {
            size_t start = 0;
            if (buf[start] == 6)
            {
                start++, n--;
                field->FieldType(GetType(lib, assembly, reader, buf, start, n));
            }
        }
    }
} // namespace