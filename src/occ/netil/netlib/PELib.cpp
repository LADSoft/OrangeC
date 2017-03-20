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
#include "DLLExportReader.h"

namespace DotNetPELib
{
    extern std::string DIR_SEP;
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
        outputStream_ = new std::fstream(file.c_str(), std::ios::in | std::ios::out | std::ios::trunc | (mode == ilasm ? 0 : std::ios::binary));
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
    bool PELib::AddUsing(std::string path)
    {
        std::vector<std::string> split;
        SplitPath(split, path);
        bool found = false;
        for (auto a : assemblyRefs_)
        {
            size_t n = 0;
            DataContainer *container = a->FindContainer(split, n);
            if (n == split.size() && container && typeid(*container) == typeid(Namespace))
            {
                usingList_.push_back(static_cast<Namespace *>(container));
                found = true;
            }
        }
        return found;
    }
    void PELib::SplitPath(std::vector<std::string> & split, std::string path)
    {
        std::string last;
        int n = path.find(":");
        if (n != std::string::npos && n < path.size() - 2 && path[n+1] == ':')
        {
            last = path.substr(n + 2);
            path = path.substr(0, n);
        }
        n = path.find(".");
        while (n != std::string::npos)
        {
            split.push_back(path.substr(0,n));
            if (path.size() > n + 1)
                path = path.substr(n+1);
            else
                path = "";
            n = path.find(".");
        }
        if (path.size())
        {
            split.push_back(path);
        }
        if (last.size())
        {
            split.push_back(last);
        }
    }
    PELib::eFindType PELib::Find(std::string path, void **result, AssemblyDef *assembly)
    {
        std::vector<std::string> split;
        SplitPath(split, path);
        std::vector<DataContainer *> found;
        std::vector<Field *> foundField;
        std::vector<Method *> foundMethod;
        std::vector<Property *> foundProperty;

        for (auto a : assemblyRefs_)
        {
            if (a->InAssemblyRef())
            {
                if (!assembly || assembly == a)
                {
                    size_t n = 0;
                    DataContainer *dc = a->FindContainer(split, n);
                    if (dc)
                    {
                        if (n == split.size())
                        {
                            found.push_back(dc);
                        }
                        else if (n == split.size() - 1 && (typeid(*dc) == typeid(Class) || typeid(*dc) == typeid(Enum)))
                        {
                            for (auto field : dc->Fields())
                            {
                                if (field->Name() == split[n])
                                    foundField.push_back(field);
                            }
                            for (auto cc : dc->Methods())
                            {
                                Method *method = static_cast<Method *>(cc);
                                if (method->Signature()->Name() == split[n])
                                    foundMethod.push_back(method);
                            }
                            if (typeid(*dc) == typeid(Class))
                            {
                                for (auto cc : static_cast<Class *>(dc)->Properties())
                                {
                                    if (cc->Name() == split[n])
                                        foundProperty.push_back(cc);
                                }
                            }
                        }
                    }
                }
            }
        }
        for (auto u : usingList_)
        {
            size_t n = 0;
            DataContainer *dc = u->FindContainer(split, n);
            if (dc)
            {
                if (n == split.size())
                {
                    found.push_back(dc);
                }
                else if (n == split.size() - 1 && (typeid(*dc) == typeid(Class) || typeid(*dc) == typeid(Enum)))
                {
                    for (auto field : dc->Fields())
                    {
                        if (field->Name() == split[n])
                            foundField.push_back(field);
                    }
                    for (auto cc : dc->Methods())
                    {
                        Method *method = static_cast<Method *>(cc);
                        if (method->Signature()->Name() == split[n])
                            foundMethod.push_back(method);
                    }
                    if (typeid(*dc) == typeid(Class))
                    {
                        for (auto cc : static_cast<Class *>(dc)->Properties())
                        {
                            if (cc->Name() == split[n])
                                foundProperty.push_back(cc);
                        }
                    }
                }
            }
        }
        int n = found.size() + foundField.size() + foundMethod.size() + foundProperty.size();
        if (!n )
            return s_notFound;
        else if (n > 1)
            return s_ambiguous;
        else if (found.size())
        {
            *result = found[0];
            if (typeid(*found[0]) == typeid(Namespace))
                return s_namespace;
            else if (typeid(*found[0]) == typeid(Class))
                return s_class;
            else if (typeid(*found[0]) == typeid(Enum))
                return s_enum;
        }
        else if (foundMethod.size())
        {
            *result = foundMethod[0];
            return s_method;
        }
        else if (foundField.size())
        {
            *result = foundField[0];
            return s_field;
        }
        else if (foundProperty.size())
        {
            *result = foundProperty[0];
            return s_property;
        }
        return s_notFound;

    }
    PELib::eFindType PELib::Find(std::string path, Method **result, std::vector<Type *> args, AssemblyDef *assembly)
    {
        std::vector<std::string> split;
        SplitPath(split, path);
        std::vector<Method *> foundMethod;

        for (auto a : assemblyRefs_)
        {
            if (a->InAssemblyRef())
            {
                if (!assembly || assembly == a)
                {
                    size_t n = 0;
                    DataContainer *dc = a->FindContainer(split, n);
                    if (dc)
                    {
                        if (n == split.size() - 1 && typeid(*dc) == typeid(Class) || typeid(*dc) == typeid(Enum))
                        {
                            for (auto cc : dc->Methods())
                            {
                                Method *method = static_cast<Method *>(cc);
                                if (method->Signature()->Name() == split[n])
                                    foundMethod.push_back(method);
                            }
                        }
                    }
                }
            }
        }
        for (auto u : usingList_)
        {
            size_t n = 0;
            DataContainer *dc = u->FindContainer(split, n);
            if (dc)
            {
                if (n == split.size() - 1 && typeid(*dc) == typeid(Class) || typeid(*dc) == typeid(Enum))
                {
                    for (auto cc : dc->Methods())
                    {
                        Method *method = static_cast<Method *>(cc);
                        if (method->Signature()->Name() == split[n])
                            foundMethod.push_back(method);
                    }
                }
            }
        }
        for (auto it = foundMethod.begin(); it != foundMethod.end() ;)
        {
            if (!(*it)->Signature()->Matches(args))
                it = foundMethod.erase(it);
            else
                ++it;
        }
        if (foundMethod.size() > 1)
        {
            for (auto it = foundMethod.begin(); it != foundMethod.end();)
            {
                if ((*it)->Signature()->Flags() & MethodSignature::Vararg)
                    it = foundMethod.erase(it);
                else
                    ++it;
            }
        }
        if (foundMethod.size() == 0)
        {
            return s_notFound;
        }
        else if (foundMethod.size() > 1)
        {
            return s_ambiguous;
        }
        else
        {
            *result = foundMethod[0];
            return s_method;
        }
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
    Class *PELib::LookupClass(PEReader &reader, std::string assemblyName, int major,
                             int minor, int build, int revision, size_t publicKeyIndex, 
                             std::string nameSpace, std::string name)
    {
        AssemblyDef *assembly = FindAssembly(assemblyName);
        if (!assembly)
        {
            AddExternalAssembly(assemblyName);
            assembly = FindAssembly(assemblyName);
            assembly->SetVersion(major, minor, build, revision);
            if (publicKeyIndex)
            {
                assembly->SetPublicKey (reader, publicKeyIndex);
            }
        }
        return assembly->LookupClass(*this, nameSpace, name);
    }
    bool PELib::DumpPEFile(std::string file, bool isexe, bool isgui)
    {
        int n = 1;
        WorkingAssembly()->Number(n); // give initial PE Indexes for field resolution..

        peWriter_ = new PEWriter(isexe, isgui, WorkingAssembly()->SNKFile());
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
            AssemblyDef *mscorlibAssembly = MSCorLibAssembly();
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
    AssemblyDef *PELib::MSCorLibAssembly()
    {
        AssemblyDef *mscorlibAssembly = FindAssembly("mscorlib");
        if (mscorlibAssembly == nullptr)
        {
            LoadAssembly("mscorlib");
            mscorlibAssembly = FindAssembly("mscorlib");
        }
        return mscorlibAssembly;
    }
    int PELib::LoadAssembly(std::string assemblyName)
    {
        AssemblyDef *assembly = FindAssembly(assemblyName);
        if (assembly == nullptr || !assembly->IsLoaded())
        {
            PEReader r;
            int n = r.ManagedLoad(assemblyName + ".dll");
            if (!n)
            {
                if (!assembly)
                    AddExternalAssembly(assemblyName);
                assembly = FindAssembly(assemblyName);
                assembly->Load(*this, r);
                assembly->SetLoaded();
            }
            return n;
        }
        return 0;
    }
    int PELib::LoadUnmanaged(std::string name)
    {
        DLLExportReader reader(name.c_str());
        if (reader.Read())
        {
            std::string unmanagedDllName = reader.Name();
            unsigned npos = unmanagedDllName.find_last_of(DIR_SEP);
            if (npos != std::string::npos && npos != unmanagedDllName.size()-1)
            {
                unmanagedDllName = unmanagedDllName.substr(npos+1);
            }
            for (DLLExportReader::iterator it = reader.begin(); it != reader.end(); ++it)
            {
                unmanagedRoutines_[(*it)->name] = unmanagedDllName;
            }
            return 0;
        }
        return 1;
    }
    std::string PELib::FindUnmanagedName(std::string name)
    {
        return unmanagedRoutines_[name];
    }
}
