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

    PELib::PELib(std::string AssemblyName, int CoreFlags) 
        : DataContainer("", 0), corFlags(CoreFlags), peWriter(NULL)
    {
        AssemblyDef *assemblyRef = AllocateAssemblyDef(AssemblyName, false);
        assemblyRefs.push_back(assemblyRef);
    }
    bool PELib::DumpOutputFile(std::string file, OutputMode mode)
    {
        bool rv;
        outputStream = new std::fstream(file.c_str(), std::ios::out);
        switch (mode)
        {
            case ilasm:
                rv = ILSrcDump(*this);
                break;
            case peexe:
                rv = DumpPEFile(file, true);
                break;
            case pedll:
                rv = DumpPEFile(file, false);
                break;
            default:
                rv = false;
                break;
        }
        delete outputStream;
        return rv;
    }
    void PELib::AddExternalAssembly(std::string assemblyName)
    {
        AssemblyDef *assemblyRef = AllocateAssemblyDef(assemblyName, true);
        assemblyRefs.push_back(assemblyRef);
    }
    void PELib::AddPInvokeReference(MethodSignature *methodsig, std::string dllname, bool iscdecl)
    {
        Method *m = AllocateMethod(methodsig, Qualifiers::PInvokeFunc | Qualifiers::Public);
        m->SetPInvoke(dllname, iscdecl ? Method::Cdecl : Method::Stdcall);
        pInvokeSignatures.push_back(m);
    }
    bool PELib::ILSrcDumpHeader()
    {
        *outputStream << ".corflags " << corFlags <<std::endl << std::endl;
        for (std::list<AssemblyDef *>::iterator it = assemblyRefs.begin(); it != assemblyRefs.end(); ++it)
        {
            (*it)->ILSrcDump(*this);
        }
        *outputStream << std::endl;
        return true;
    }
    bool PELib::ILSrcDumpFile()
    {
        bool rv = DataContainer::ILSrcDump(*this);
        if (rv)
        {
            for (std::list<Method *>::iterator it = pInvokeSignatures.begin(); it != pInvokeSignatures.end(); ++it)
            {
                (*it)->ILSrcDump(*this);
            }
        }
        return rv;
    }
    AssemblyDef *PELib::FindAssembly(std::string assemblyName)
    {
        for (std::list<AssemblyDef *>::iterator it = assemblyRefs.begin(); it != assemblyRefs.end(); ++it)
        {
            if ((*it)->GetName() == assemblyName)
                return *it;
        }
        return NULL;
    }
    bool PELib::DumpPEFile(std::string file, bool isexe)
    {
        peWriter = new PEWriter(isexe);
        size_t moduleIndex = peWriter->HashString("<Module>");
        TypeDefOrRef typeDef(TypeDefOrRef::TypeDef, 0);
        TableEntryBase *table = new TypeDefTableEntry(0,moduleIndex, 0, typeDef, 1, 1);
        peWriter->AddTableEntry(table);

        int types = 0;
        GetBaseTypes(types);
        if (types)
        {
            AssemblyDef *mscorlibAssembly = FindAssembly("mscorlib");
            if (mscorlibAssembly == NULL)
            {
                AllocateAssemblyDef("mscorlib", true);
            }
        }
        size_t systemIndex = peWriter->HashString("System");
        size_t objectIndex = 0;
        size_t valueIndex = 0;
        size_t enumIndex = 0;
        if (types)
        {
            if (types & basetypeObject)
            {
                objectIndex = peWriter->HashString("Object");
            }
            if (types & basetypeValue)
            {
                valueIndex = peWriter->HashString("ValueType");
            }
            if (types & basetypeEnum)
            {
                enumIndex = peWriter->HashString("Enum");
            }
        }
        for (std::list<AssemblyDef *>::iterator it = assemblyRefs.begin(); it != assemblyRefs.end(); ++it)
        {
            (*it)->PEDump(*this);
        }
        if (types)
        {
            AssemblyDef *mscorlibAssembly = FindAssembly("mscorlib");
            int assemblyIndex = mscorlibAssembly->GetPEIndex();
            ResolutionScope rs(ResolutionScope::AssemblyRef, assemblyIndex );
            if (types & basetypeObject)
            {
                table = new TypeRefTableEntry(rs, objectIndex, systemIndex);
                objectIndex = peWriter->AddTableEntry(table);
            }
            if (types & basetypeValue)
            {
                table = new TypeRefTableEntry(rs, valueIndex, systemIndex);
                valueIndex = peWriter->AddTableEntry(table);
            }
            if (types & basetypeEnum)
            {
                table = new TypeRefTableEntry(rs, enumIndex, systemIndex);
                enumIndex = peWriter->AddTableEntry(table);
            }
            peWriter->SetBaseClasses(objectIndex, valueIndex, enumIndex, systemIndex);
        }
        size_t npos = file.find_last_of("\\");
        if (npos != std::string::npos && npos != file.size()-1)
            file = file.substr(npos + 1);
        size_t nameIndex = peWriter->HashString(file);
        unsigned char guid[128/8];
        peWriter->CreateGuid(guid);
        size_t guidIndex = peWriter->HashGUID(guid);
        table = new ModuleTableEntry(nameIndex, guidIndex);
        peWriter->AddTableEntry(table);


        bool rv = DataContainer::PEDump(*this);
        for (std::list<Method *>::iterator it = pInvokeSignatures.begin(); it != pInvokeSignatures.end(); ++it)
        {
            (*it)->PEDump(*this);
        }
        Compile(*this);
        peWriter->WriteFile(*this, *outputStream);
        delete peWriter;
        return false;
    }
}
