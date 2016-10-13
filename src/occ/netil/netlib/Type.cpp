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
#include <stdio.h>
namespace DotNetPELib
{

    char *Type::typeNames[] = {
        "", "", "void", "int8", "uint8", "int16", "uint16", "int32", "uint32", "int64", "uint64", "int", "uint",
        "float32", "float64", "object", "object []", "string" 
    };    
    char *BoxedType::typeNames[] = { "", "", "", "Int8", "UInt8",
        "Int16", "UInt16", "Int32", "UInt32",
        "Int64", "UInt64", "Int", "UInt", "Float", "Double"
    };
    bool Type::ILSrcDump(PELib &peLib)
    {
        if (fullName.size())
        {
            peLib.Out() << fullName;
        }
        else if (tp == cls)
        {
            peLib.Out() << "'" << Qualifiers::GetName("", typeRef, true) << "'";
        }
        else if (tp == method)
        {
            peLib.Out() << "method ";
            methodRef->ILSrcDump(peLib, false, true, true);
        }
        else
        {
            peLib.Out() << typeNames[tp];
        }
        for (int i=0; i < pointerLevel; i++)
            peLib.Out() << " *";
        return true;
    }
    size_t Type::Render(PELib &peLib, unsigned char *result)
    {
        if (fullName.size())
        {
            if (!peIndex)
            {
                char assemblyName[1024],namespaceName[1024],className[1024];
                if (sscanf(fullName.c_str(), "[%[^]]]%[^.].%s", assemblyName, namespaceName, className) == 3)
                {
                    AssemblyDef *assembly = peLib.FindAssembly(assemblyName);
                    if (!assembly)
                    {
                        peLib.AllocateAssemblyDef(assemblyName, true);
                        assembly = peLib.FindAssembly(assemblyName);
                        assembly->PEDump(peLib);
                    }
                    size_t nmspc = peLib.PEOut().HashString(namespaceName);
                    size_t cls = peLib.PEOut().HashString(className);
                    ResolutionScope rs(ResolutionScope::AssemblyRef, assembly->GetPEIndex() );
                    TableEntryBase *table = new TypeRefTableEntry(rs, cls, nmspc);
                    peIndex = peLib.PEOut().AddTableEntry(table);
                }
            }
            *(int *)result = peIndex | (tTypeRef << 24);
            return 4;
        }
        else switch(tp)
        {
            case cls:
                *(int *)result = typeRef->GetPEIndex() | (tTypeDef << 24);
                return 4;
                break;
            case method:
            default:
            {
                if (!peIndex)
                {
                    size_t sz;
                    unsigned char *sig = SignatureGenerator::TypeSig(this, sz);
                    size_t signature = peLib.PEOut().HashBlob(sig, sz);
                    delete[] sig;
                    TypeSpecTableEntry *table = new TypeSpecTableEntry(signature);
                    peIndex = peLib.PEOut().AddTableEntry(table);
                }
                *(int *)result = peIndex | (tTypeSpec << 24);
                return 4;
            }
                break;
        }
        return true;
    }
    bool BoxedType::ILSrcDump(PELib &peLib)
    {
        peLib.Out() << "[mscorlib]System." << typeNames[tp];
        return true;
    }
    size_t BoxedType::Render(PELib &peLib, unsigned char *result)
    {
        if (!peIndex)
        {
            size_t system = peLib.PEOut().SystemName();
            size_t name = peLib.PEOut().HashString(typeNames[tp]);
            AssemblyDef *assembly = peLib.FindAssembly("mscorlib");
            ResolutionScope rs(ResolutionScope::AssemblyRef, assembly->GetPEIndex() );
            TableEntryBase *table = new TypeRefTableEntry(rs, name, system);
            peIndex = peLib.PEOut().AddTableEntry(table);
        }
        *(int *)result = peIndex | (tTypeRef << 24);
        return 4;
    }
}
