/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#include "DotNetPELib.h"
#include "PEFile.h"
#include "DLLExportReader.h"

#define OBJECT_FILE_VERSION "100"

namespace DotNetPELib
{

extern std::string DIR_SEP;
PELib::PELib(const std::string& AssemblyName, int CoreFlags) :
    corFlags_(CoreFlags),
    peWriter_(nullptr),
    inputStream_(nullptr),
    outputStream_(nullptr),
    codeContainer_(nullptr),
    objInputBuf_(nullptr),
    objInputSize_(0),
    objInputPos_(0),
    objInputCache_(0)
{
    // create the working assembly.   Note that this will ALWAYS be the first
    // assembly in the list
    AssemblyDef* assemblyRef = AllocateAssemblyDef(AssemblyName, false);
    assemblyRefs_.push_back(assemblyRef);
}
AssemblyDef* PELib::EmptyWorkingAssembly(const std::string& AssemblyName)
{
    AssemblyDef* assemblyRef = AllocateAssemblyDef(AssemblyName, false);
    assemblyRefs_.pop_front();
    assemblyRefs_.push_front(assemblyRef);
    return assemblyRef;
}
bool PELib::DumpOutputFile(const std::string& file, OutputMode mode, bool gui)
{
    bool rv;
    outputStream_ = std::make_unique<std::fstream>(file.c_str(), std::ios::in | std::ios::out | std::ios::trunc |
                                                       (mode == ilasm || mode == object ? std::ios::in : std::ios::binary));
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
        case object:
            rv = ObjOut();
            break;
        default:
            rv = false;
            break;
    }
    static_cast<std::fstream&>(*outputStream_).close();
    return rv;
}
void PELib::AddExternalAssembly(const std::string& assemblyName, Byte* publicKeyToken)
{
    AssemblyDef* assemblyRef = AllocateAssemblyDef(assemblyName, true, publicKeyToken);
    assemblyRefs_.push_back(assemblyRef);
}
void PELib::AddPInvokeReference(MethodSignature* methodsig, const std::string& dllname, bool iscdecl)
{
    Method* m = AllocateMethod(methodsig, Qualifiers::PInvokeFunc | Qualifiers::Public);
    m->SetPInvoke(dllname, iscdecl ? Method::Cdecl : Method::Stdcall);
    pInvokeSignatures_[methodsig->Name()] = m;
}
Method* PELib::FindPInvoke(const std::string& name) const
{
    auto it = pInvokeSignatures_.find(name);
    if (it != pInvokeSignatures_.end())
        return it->second;
    return nullptr;
}
MethodSignature* PELib::FindPInvokeWithVarargs(const std::string& name, std::vector<Param*>& vargs) const
{
    auto range = pInvokeReferences_.equal_range(name);
    for (auto it = range.first; it != range.second; ++it)
    {
        if (vargs.size() == (*it).second->VarargParamCount())
        {
            auto it1 = (*it).second->vbegin();
            auto it2 = vargs.begin();
            while (it2 != vargs.end())
            {
                if (!(*it2)->GetType()->Matches((*it1)->GetType()))
                    break;
                ++it1;
                ++it2;
            }
            if (it2 == vargs.end())
                return (*it).second;
        }
    }
    return nullptr;
}
bool PELib::AddUsing(const std::string& path)
{
    std::vector<std::string> split;
    SplitPath(split, path);
    bool found = false;
    for (auto a : assemblyRefs_)
    {
        size_t n = 0;
        DataContainer* container = a->FindContainer(split, n);
        if (n == split.size() && container && typeid(*container) == typeid(Namespace))
        {
            usingList_.push_back(static_cast<Namespace*>(container));
            found = true;
        }
    }
    return found;
}
void PELib::SplitPath(std::vector<std::string>& split, std::string path)
{
    std::string last;
    int n = path.find(":");
    if (n != std::string::npos && n < path.size() - 2 && path[n + 1] == ':')
    {
        last = path.substr(n + 2);
        path = path.substr(0, n);
    }
    n = path.find(".");
    while (n != std::string::npos)
    {
        split.push_back(path.substr(0, n));
        if (path.size() > n + 1)
            path = path.substr(n + 1);
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
    if (split.size() > 2)
    {
        if (split[split.size() - 1] == "ctor" || split[split.size() - 1] == "cctor")
            if (split[split.size() - 2] == "")
            {
                split[split.size() - 2] = "." + split[split.size() - 1];
                split.resize(split.size() - 1);
            }
    }
}
PELib::eFindType PELib::Find(std::string path, void **result, std::deque<Type*>* generics, AssemblyDef *assembly)
{
    if (path.size() && path[0] == '[')
    {
        size_t npos = path.find(']');
        if (npos != std::string::npos)
        {
            std::string assemblyName = path.substr(1, npos - 1);
            path = path.substr(npos + 1);
            assembly = FindAssembly(assemblyName);
        }
    }
    std::vector<std::string> split;
    SplitPath(split, path);
    std::vector<DataContainer*> found;
    std::vector<Field*> foundField;
    std::vector<Method*> foundMethod;
    std::vector<Property*> foundProperty;

    for (auto a : assemblyRefs_)
    {
        //            if (a->InAssemblyRef())
        {
            if (!assembly || assembly == a)
            {
                size_t n = 0;
                DataContainer* dc = a->FindContainer(split, n, generics);
                if (dc)
                {
                    if (n == split.size())
                    {
                        found.push_back(dc);
                    }
                    else if (n == split.size() - 1 &&
                             (typeid(*dc) == typeid(Class) || typeid(*dc) == typeid(Enum) || typeid(*dc) == typeid(AssemblyDef)))
                    {
                        for (auto field : dc->Fields())
                        {
                            if (field->Name() == split[n])
                                foundField.push_back(field);
                        }
                        for (auto cc : dc->Methods())
                        {
                            Method* method = static_cast<Method*>(cc);
                            if (method->Signature()->Name() == split[n])
                                foundMethod.push_back(method);
                        }
                        if (typeid(*dc) == typeid(Class))
                        {
                            for (auto cc : static_cast<Class*>(dc)->Properties())
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
        DataContainer* dc = u->FindContainer(split, n, generics);
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
                    Method* method = static_cast<Method*>(cc);
                    if (method->Signature()->Name() == split[n])
                        foundMethod.push_back(method);
                }
                if (typeid(*dc) == typeid(Class))
                {
                    for (auto cc : static_cast<Class*>(dc)->Properties())
                    {
                        if (cc->Name() == split[n])
                            foundProperty.push_back(cc);
                    }
                }
            }
        }
    }
    int n = found.size() + foundField.size() + foundMethod.size() + foundProperty.size();
    if (!n)
        return s_notFound;
    else if (n > 1)
    {
        return s_ambiguous;
    }
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
Class* PELib::FindOrCreateGeneric(std::string name, std::deque<Type*>& generics)
{
    void *result = nullptr;
    if (Find(name, &result, &generics) == s_class)
    {
        return static_cast<Class *>(result);
    }
    if (Find(name, &result) == s_class)
    {
        Class* rv = AllocateClass(static_cast<Class*>(result));
        rv->Generic() = generics;
        rv->GenericParent(static_cast<Class*>(result));
        static_cast<Class *>(result)->Parent()->Add(rv);
        rv->Clear();
        for (auto m : static_cast<Class*>(result)->Methods())
        {
            // only doing methods right now...
            Method *old = static_cast<Method*>(m);
            MethodSignature *m1 = AllocateMethodSignature(old->Signature());
            m1->SetContainer(rv);
            Method* nm = AllocateMethod(m1, old->Flags());
            rv->Add(nm);
        }
        return rv;
    }
    return nullptr;
}

PELib::eFindType PELib::Find(std::string path, Method **result, std::vector<Type *> args, Type* rv, std::deque<Type*>* generics, AssemblyDef *assembly, bool matchArgs)
{
    if (path.size() && path[0] == '[')
    {
        size_t npos = path.find(']');
        if (npos != std::string::npos)
        {
            std::string assemblyName = path.substr(1, npos - 1);
            path = path.substr(npos + 1);
            assembly = FindAssembly(assemblyName);
        }
    }
    std::vector<std::string> split;
    SplitPath(split, path);
    std::vector<Method*> foundMethod;

    for (auto a : assemblyRefs_)
    {
        //            if (a->InAssemblyRef())
        {
            if (!assembly || assembly == a)
            {
                size_t n = 0;
                DataContainer* dc = a->FindContainer(split, n, generics, true);
                if (dc)
                {
                    if (n == split.size() - 1 &&
                        (typeid(*dc) == typeid(Class) || typeid(*dc) == typeid(Enum) || typeid(*dc) == typeid(AssemblyDef)))
                    {
                        for (auto cc : dc->Methods())
                        {
                            Method* method = static_cast<Method*>(cc);
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
        DataContainer* dc = u->FindContainer(split, n, generics);
        if (dc)
        {
            if ((n == split.size() - 1 && typeid(*dc) == typeid(Class)) || typeid(*dc) == typeid(Enum))
            {
                for (auto cc : dc->Methods())
                {
                    Method* method = static_cast<Method*>(cc);
                    if (method->Signature()->Name() == split[n])
                        foundMethod.push_back(method);
                }
            }
        }
    }
    if (matchArgs)
    {
        for (auto it = foundMethod.begin(); it != foundMethod.end();)
        {
            if (!(*it)->Signature()->Matches(args) || (rv && !(*it)->Signature()->MatchesType((*it)->Signature()->ReturnType(), rv)))
                it = foundMethod.erase(it);
            else
                ++it;
        }
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
        Method* method = FindPInvoke(path);
        if (method)
        {
            *result = method;
            return s_method;
        }
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
    for (std::list<AssemblyDef*>::const_iterator it = assemblyRefs_.begin(); it != assemblyRefs_.end(); ++it)
    {
        (*it)->ILHeaderDump(*this);
    }
    *outputStream_ << std::endl;
    return true;
}
bool PELib::ILSrcDumpFile()
{
    WorkingAssembly()->ILSrcDump(*this);
    for (auto sig : pInvokeSignatures_)
    {
        sig.second->ILSrcDump(*this);
    }
    return true;
}
bool PELib::ObjOut()
{
    *outputStream_ << "$qb" << OBJECT_FILE_VERSION << "," << corFlags_;
    for (auto a : assemblyRefs_)
    {
        // classes and namespaces
        a->ObjOut(*this, 1);
    }
    for (auto p : pInvokeSignatures_)
    {
        // pinvoke signatures
        p.second->ObjOut(*this, 2);
    }
    for (auto a : assemblyRefs_)
    {
        // method definitions and fields
        a->ObjOut(*this, 2);
    }
    for (auto a : assemblyRefs_)
    {
        // method bodies
        a->ObjOut(*this, 3);
    }
    *outputStream_ << "$qe";
    return true;
}
bool PELib::LoadObject(const std::string& name)
{
    bool rv = false;
    inputStream_ = new std::fstream(name, std::ios::in);
    if (inputStream_ && inputStream_->is_open())
        rv = ObjIn();
    delete inputStream_;
    inputStream_ = nullptr;
    return rv;
}
bool PELib::ObjIn()
{
    bool rv = false;
    containerStack_.clear();
    codeContainer_ = nullptr;

    std::string buf((std::istreambuf_iterator<char>(*inputStream_)), std::istreambuf_iterator<char>());
    objInputPos_ = 0;
    objInputBuf_ = buf.c_str();
    objInputSize_ = buf.size();

    try
    {
        if (ObjBegin() == 'q')
        {
            int ver = ObjInt();
            int ch = ObjChar();
            if (ch != ',')
                ObjError(oe_syntax);
            int corFlags = ObjInt();
            if (corFlags & PELib::ilonly)
                corFlags_ |= PELib::ilonly;
            if ((corFlags ^ corFlags_) & PELib::bits32)
                ObjError(oe_corFlagsMismatch);
            while (ObjBegin() == 'a')
            {
                AssemblyDef::ObjIn(*this);
            }
            if (ObjBegin(false) == 'm')
                do
                {
                    Method* m = Method::ObjIn(*this);
                    if (m)
                    {
                        pInvokeSignatures_[m->Signature()->Name()] = m;
                    }
                } while (ObjBegin() == 'm');
            if (ObjBegin(false) == 'a')
                do
                {
                    AssemblyDef::ObjIn(*this);
                } while (ObjBegin() == 'a');
            if (ObjEnd(false) == 'q')
            {
            }
            else
            {
                ObjError(oe_syntax);
            }
        }
        else
        {
            ObjError(oe_syntax);
        }
        rv = true;
    }
    catch (ObjectError&)
    {
    }
    objInputBuf_ = nullptr;
    objInputSize_ = 0;
    objInputPos_ = 0;
    return rv;
}
int PELib::ObjHex2()
{
    char n1, n2;
    n1 = ObjChar();
    n1 = toupper(n1);
    if (n1 < '0' || (n1 > '9' && n1 < 'A') || n1 > 'F')
    {
        objInputPos_--;
        return -1;
    }
    n2 = ObjChar();
    n2 = toupper(n2);
    if (n2 < '0' || (n2 > '9' && n2 < 'A') || n2 > 'F')
        ObjError(oe_syntax);
    n1 -= '0';
    if (n1 > 9)
        n1 -= 'A' - ('9' + 1);
    n2 -= '0';
    if (n2 > 9)
        n2 -= 'A' - ('9' + 1);
    return (n1 << 4) + n2;
}
longlong PELib::ObjInt()
{
    char buf[256], *p = buf, ch;
    bool minus = false;
    if ((ch = ObjChar()) == '-')
    {
        minus = true;
        ch = ObjChar();
    }
    while (isdigit(ch))
    {
        *p++ = ch;
        ch = ObjChar();
    }
    objInputPos_--;
    *p = 0;
    longlong value = 0;
    for (p = buf; *p; ++p)
    {
        value *= 10;
        value += (*p - '0');
    }
    if (minus)
        return -value;
    else
        return value;
}
char PELib::ObjBegin(bool next)
{
    if (!next)
        objInputPos_ = objInputCache_;
    else
        objInputCache_ = objInputPos_;
    char ch = ObjChar();
    if (ch == '$')
    {

        if (objInputBuf_[objInputPos_ + 1] == 'b')
        {

            objInputPos_ += 2;
            return objInputBuf_[objInputPos_ - 2];
        }
    }
    objInputPos_--;
    return -1;
}
char PELib::ObjEnd(bool next)
{
    if (!next)
        objInputPos_ = objInputCache_;
    else
        objInputCache_ = objInputPos_;
    char ch = ObjChar();
    if (ch == '$')
    {
        if (objInputBuf_[objInputPos_ + 1] == 'e')
        {
            objInputPos_ += 2;
            return objInputBuf_[objInputPos_ - 2];
        }
    }
    objInputPos_--;
    return -1;
}
char PELib::ObjChar()
{
    while (isspace(objInputBuf_[objInputPos_]))
        objInputPos_++;
    if (objInputPos_ < objInputSize_)
        return objInputBuf_[objInputPos_++];
    return -1;
}
void PELib::ObjError(int errnum)
{
    static char buf[256];
    sprintf(buf, "%d", errnum);
    ObjectError a(buf);
    throw a;
}
std::string PELib::UnformatName()
{
    int n1 = ObjHex2();
    if (n1 < 0)
        ObjError(oe_syntax);
    int n2 = ObjHex2();
    if (n2 < 0)
        ObjError(oe_syntax);
    n1 = (n1 << 8) + n2;

    std::string nn(objInputBuf_ + objInputPos_, n1);
    objInputPos_ += n1;
    return nn;
}
std::string PELib::FormatName(const std::string& name)
{
    char buf[256];
    sprintf(buf, "%04X", (unsigned)name.size());
    return std::string(buf) + name;
}
AssemblyDef* PELib::FindAssembly(const std::string& assemblyName) const
{
    for (std::list<AssemblyDef*>::const_iterator it = assemblyRefs_.begin(); it != assemblyRefs_.end(); ++it)
    {
        if ((*it)->Name() == assemblyName)
            return *it;
    }
    return nullptr;
}
Class* PELib::LookupClass(PEReader& reader, const std::string& assemblyName, int major, int minor, int build, int revision,
                          size_t publicKeyIndex, const std::string& nameSpace, const std::string& name)
{
    AssemblyDef* assembly = FindAssembly(assemblyName);
    if (!assembly)
    {
        AddExternalAssembly(assemblyName);
        assembly = FindAssembly(assemblyName);
        assembly->SetVersion(major, minor, build, revision);
        if (publicKeyIndex)
        {
            assembly->SetPublicKey(reader, publicKeyIndex);
        }
    }
    return assembly->LookupClass(*this, nameSpace, name);
}

bool PELib::DumpPEFile(std::string file, bool isexe, bool isgui)
{
    int n = 1;
    WorkingAssembly()->Number(n);  // give initial PE Indexes for field resolution..

    peWriter_ = new PEWriter(isexe, isgui, WorkingAssembly()->SNKFile());
    size_t moduleIndex = peWriter_->HashString("Module");
    TypeDefOrRef typeDef(TypeDefOrRef::TypeDef, 0);
    TableEntryBase* table = new TypeDefTableEntry(0, moduleIndex, 0, typeDef, 1, 1);
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
        AssemblyDef* mscorlibAssembly = MSCorLibAssembly();
        int assemblyIndex = mscorlibAssembly->PEIndex();
        ResolutionScope rs(ResolutionScope::AssemblyRef, assemblyIndex);
        if (types & DataContainer::basetypeObject)
        {
            void* result = nullptr;
            table = new TypeRefTableEntry(rs, objectIndex, systemIndex);
            objectIndex = peWriter_->AddTableEntry(table);
            Find("[mscorlib]System::Object", &result);
            if (result)
                static_cast<Class*>(result)->PEIndex(objectIndex);
        }
        if (types & DataContainer::basetypeValue)
        {
            void* result = nullptr;
            table = new TypeRefTableEntry(rs, valueIndex, systemIndex);
            valueIndex = peWriter_->AddTableEntry(table);
            Find("[mscorlib]System::ValueType", &result);
            if (result)
                static_cast<Class*>(result)->PEIndex(valueIndex);
        }
        if (types & DataContainer::basetypeEnum)
        {
            void* result = nullptr;
            table = new TypeRefTableEntry(rs, enumIndex, systemIndex);
            enumIndex = peWriter_->AddTableEntry(table);
            Find("[mscorlib]System::Enum", &result);
            if (result)
                static_cast<Class*>(result)->PEIndex(enumIndex);
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
        signature.second->PEDump(*this);
    }
    bool rv = WorkingAssembly()->PEDump(*this);
    WorkingAssembly()->Compile(*this);
    peWriter_->WriteFile(*this, *outputStream_);
    delete peWriter_;
    return rv;
}
AssemblyDef* PELib::MSCorLibAssembly()
{
    AssemblyDef* mscorlibAssembly = FindAssembly("mscorlib");
    if (mscorlibAssembly == nullptr)
    {
        LoadAssembly("mscorlib");
        mscorlibAssembly = FindAssembly("mscorlib");
    }
    return mscorlibAssembly;
}
int PELib::LoadAssembly(const std::string& assemblyName, int major, int minor, int build, int revision)
{
    AssemblyDef* assembly = FindAssembly(assemblyName);
    if (assembly == nullptr || !assembly->IsLoaded())
    {
        PEReader r;
        int n = r.ManagedLoad(assemblyName, major, minor, build, revision);
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
int PELib::LoadUnmanaged(const std::string& name)
{
    DLLExportReader reader(name.c_str());
    if (reader.Read())
    {
        std::string unmanagedDllName = reader.Name();
        size_t npos = unmanagedDllName.find_last_of(DIR_SEP);
        if (npos != std::string::npos && npos != unmanagedDllName.size() - 1)
        {
            unmanagedDllName = unmanagedDllName.substr(npos + 1);
        }
        for (DLLExportReader::iterator it = reader.begin(); it != reader.end(); ++it)
        {
            unmanagedRoutines_[(*it)->name] = unmanagedDllName;
        }
        return 0;
    }
    return 1;
}
std::string PELib::FindUnmanagedName(const std::string& name) { return unmanagedRoutines_[name]; }
void PELib::Traverse(Callback& callback) const
{
    for (auto a : assemblyRefs_)
    {
        if (callback.EnterAssembly(a))
        {
            if (!a->Traverse(callback))
                break;
            if (!callback.ExitAssembly(a))
                break;
        }
    }
}

}  // namespace DotNetPELib
