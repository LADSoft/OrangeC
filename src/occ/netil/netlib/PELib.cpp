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
    Byte PELib::defaultPublicKeyToken_[] = { 0xB7, 0x7A, 0x5C, 0x56, 0x19, 0x34, 0xE0, 0x89 };

    PELib::PELib(std::string AssemblyName, int CoreFlags)
        : corFlags_(CoreFlags), peWriter_(nullptr)
    {
        // create the working assembly.   Note that this will ALWAYS be the first
        // assembly in the list
        AssemblyDef *assemblyRef = AllocateAssemblyDef(AssemblyName, false);
        assemblyRefs_.push_back(assemblyRef);
    }
    bool PELib::DumpOutputFile(std::string file, OutputMode mode, bool gui)
    {
        bool rv;
        outputStream_ = new std::fstream(file.c_str(), std::ios::out | (mode == ilasm ? 0 : std::ios::binary));
        switch (mode)
        {
        case ilasm:
            rv = ILSrcDump(*this);
            break;
        case peexe:
            rv = DumpPEFile(file, true, gui);
            break;
        case pedll:
            rv = DumpPEFile(file, false, gui);
            break;
        default:
            rv = false;
            break;
        }
        delete outputStream_;
        return rv;
    }
    void PELib::AddExternalAssembly(std::string assemblyName, Byte *publicKeyToken)
    {
        AssemblyDef *assemblyRef = AllocateAssemblyDef(assemblyName, true, publicKeyToken);
        assemblyRefs_.push_back(assemblyRef);
    }
    void PELib::AddPInvokeReference(MethodSignature *methodsig, std::string dllname, bool iscdecl)
    {
        Method *m = AllocateMethod(methodsig, Qualifiers::PInvokeFunc | Qualifiers::Public);
        m->SetPInvoke(dllname, iscdecl ? Method::Cdecl : Method::Stdcall);
        pInvokeSignatures_.push_back(m);
    }
    bool PELib::ILSrcDumpHeader()
    {
        *outputStream_ << ".corflags " << corFlags_ << std::endl << std::endl;
        for (std::list<AssemblyDef *>::const_iterator it = assemblyRefs_.begin(); it != assemblyRefs_.end(); ++it)
        {
            (*it)->ILHeaderDump(*this);
        }
        *outputStream_ << std::endl;
        return true;
    }
    bool PELib::ILSrcDumpFile()
    {
        WorkingAssembly()->ILSrcDump(*this);
        for (std::list<Method *>::const_iterator it = pInvokeSignatures_.begin(); it != pInvokeSignatures_.end(); ++it)
        {
            (*it)->ILSrcDump(*this);
        }
        return true;
    }
    AssemblyDef *PELib::FindAssembly(std::string assemblyName) const
    {
        for (std::list<AssemblyDef *>::const_iterator it = assemblyRefs_.begin(); it != assemblyRefs_.end(); ++it)
        {
            if ((*it)->Name() == assemblyName)
                return *it;
        }
        return nullptr;
    }
    bool PELib::DumpPEFile(std::string file, bool isexe, bool isgui)
    {
        int n = 1;
        WorkingAssembly()->Number(n); // give initial PE Indexes for field resolution..

        peWriter_ = new PEWriter(isexe, isgui);
        size_t moduleIndex = peWriter_->HashString("<Module>");
        TypeDefOrRef typeDef(TypeDefOrRef::TypeDef, 0);
        TableEntryBase *table = new TypeDefTableEntry(0, moduleIndex, 0, typeDef, 1, 1);
        peWriter_->AddTableEntry(table);

        int types = 0;
        WorkingAssembly()->BaseTypes(types);
        if (types)
        {
            MSCorLibAssembly();
        }
        size_t systemIndex = 0;
        size_t objectIndex = 0;
        size_t valueIndex = 0;
        size_t enumIndex = 0;
        if (types)
        {
            systemIndex = peWriter_->HashString("System");
            if (types & DataContainer::basetypeObject)
            {
                objectIndex = peWriter_->HashString("Object");
            }
            if (types & DataContainer::basetypeValue)
            {
                valueIndex = peWriter_->HashString("ValueType");
            }
            if (types & DataContainer::basetypeEnum)
            {
                enumIndex = peWriter_->HashString("Enum");
            }
        }
        for (auto assemblyRef : assemblyRefs_)
        {
            assemblyRef->PEHeaderDump(*this);
        }
        if (types)
        {
            AssemblyDef *mscorlibAssembly = FindAssembly("mscorlib");
            int assemblyIndex = mscorlibAssembly->PEIndex();
            ResolutionScope rs(ResolutionScope::AssemblyRef, assemblyIndex);
            if (types & DataContainer::basetypeObject)
            {
                table = new TypeRefTableEntry(rs, objectIndex, systemIndex);
                objectIndex = peWriter_->AddTableEntry(table);
            }
            if (types & DataContainer::basetypeValue)
            {
                table = new TypeRefTableEntry(rs, valueIndex, systemIndex);
                valueIndex = peWriter_->AddTableEntry(table);
            }
            if (types & DataContainer::basetypeEnum)
            {
                table = new TypeRefTableEntry(rs, enumIndex, systemIndex);
                enumIndex = peWriter_->AddTableEntry(table);
            }
            peWriter_->SetBaseClasses(objectIndex, valueIndex, enumIndex, systemIndex);
        }
        size_t npos = file.find_last_of("\\");
        if (npos != std::string::npos && npos != file.size() - 1)
            file = file.substr(npos + 1);
        size_t nameIndex = peWriter_->HashString(file);
        Byte guid[128 / 8];
        peWriter_->CreateGuid(guid);
        size_t guidIndex = peWriter_->HashGUID(guid);
        table = new ModuleTableEntry(nameIndex, guidIndex);
        peWriter_->AddTableEntry(table);


        for (auto signature : pInvokeSignatures_)
        {
            signature->PEDump(*this);
        }
        bool rv = WorkingAssembly()->PEDump(*this);
        WorkingAssembly()->Compile(*this);
        peWriter_->WriteFile(*this, *outputStream_);
        delete peWriter_;
        return false;
    }
    AssemblyDef *PELib::MSCorLibAssembly(bool dump)
    {
        AssemblyDef *mscorlibAssembly = FindAssembly("mscorlib");
        if (mscorlibAssembly == nullptr)
        {
            AddExternalAssembly("mscorlib");
            mscorlibAssembly = FindAssembly("mscorlib");
            if (dump)
                mscorlibAssembly->PEDump(*this);
        }
        return mscorlibAssembly;
    }
}
