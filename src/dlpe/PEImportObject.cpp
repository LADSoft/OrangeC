/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 *
 */

#include "PEObject.h"
#include "Utils.h"
#include "ObjSymbol.h"
#include "ObjFile.h"
#include "ObjExpression.h"
#include <tuple>
#include <algorithm>
#include <cctype>
#include <map>
#include <cstring>

template <class Directory>
bool DllImports<Directory>::IsDelayLoad(const std::vector<std::string>& delayLoadNames, std::string& name)
{
    for (auto&& d : delayLoadNames)
    {
        size_t n = d.find_last_of('/');
        size_t m = d.find_last_of('\\');
        if (n > m)
            n = m;
        ;
        if (n == std::string::npos)
            n = 0;
        if (d.substr(n) == name)
            return true;
    }
    return false;
}

template <class Directory>
size_t DllImports<Directory>::ModuleSize(std::map<ObjString, ObjSymbol*> externs)
{
    directory = offset;
    directoryNames = offset + sizeOfDirectory();
    nameAddr = directoryNames + ModuleDirectoryNameSize();
    names = nameAddr + CountOfIAT() * sizeof(DWORD);
    iatAddr = names + ModuleNameSize();
    if (bindTable)
        bindAddr = iatAddr + (names - nameAddr);
    else
        bindAddr = 0;
    if (unloadTable)
        unloadAddr = (bindTable ? bindAddr : iatAddr) + (names - nameAddr);
    else
        unloadAddr = 0;
    return (unloadAddr ? unloadAddr : bindAddr ? bindAddr : iatAddr) + (names - nameAddr) - offset;
}

template <class Directory>
size_t DllImports<Directory>::ModuleDirectoryNameSize()
{
    auto rv = 0;
    for (auto&& m : modules)
    {
        rv += 1 + m.first.size();
        rv += (rv & 1);
    }
    rv = (rv + 3) & ~3;
    return rv;
}

template <class Directory>
size_t DllImports<Directory>::ModuleNameSize()
{
    auto rv = 0;
    for (auto&& m : modules)
    {
        for (auto&& e : m.second->externalNames)
        {
            rv += sizeof(HintType) + 1 + std::get<0>(e).size();
            rv += (rv & 1);
        }
    }
    rv = (rv + 3) & ~3;
    return rv;
}

template <class Directory>
size_t DllImports<Directory>::CountOfIAT()
{
    int count = 0;
    for (auto&& pair : modules)
    {
        count += pair.second->externalNames.size() + 1;
    }
    return count;
}
template <class Directory>
size_t DllImports<Directory>::CountOfModules()
{
    int count = 1 + modules.size();
    return count > 1 ? count : 0;
}

template <>
void DllImports<ImportDirectory>::TrimModules(const std::vector<std::string>& delayLoadNames)
{
    for (auto it = modules.begin(); it != modules.end();)
    {
        if (IsDelayLoad(delayLoadNames, it->second->name))
        {
            it = modules.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

template <>
void DllImports<ImportDirectory>::WriteDirectory(DWORD virtual_addr, DWORD imageBase, unsigned char* data)
{
    DWORD directoryPos = directory;
    DWORD iatPos = iatAddr;
    DWORD namePos = nameAddr;
    DWORD stringPos = directoryNames;
    for (auto&& pair : modules)
    {
        auto m = pair.second;
        ImportDirectory dir = {
            RVA(namePos), m->time, 0, RVA(stringPos), RVA(iatPos),
        };
        std::copy((char*)&dir, (char*)(&dir + 1), data + directoryPos);
        std::copy(m->name.begin(), m->name.end(), data + stringPos);

        directoryPos += sizeof(ImportDirectory);
        iatPos += (m->externalNames.size() + 1) * sizeof(DWORD);
        namePos += (m->externalNames.size() + 1) * sizeof(DWORD);
        int sz = m->name.size() + 1;
        sz += (sz & 1);
        stringPos += sz;
    }
}
template <>
void DllImports<ImportDirectory>::WriteTables(std::vector<DWORD>& thunkFixups, DWORD virtual_addr, DWORD imageBase,
                                              DWORD& thunkTableRVA, unsigned char* data)
{
    DWORD iatPos = iatAddr;
    DWORD namePos = nameAddr;
    DWORD stringPos = names;
    DWORD *iatPointer = (DWORD*)(data + iatPos), *iatBase = iatPointer;
    DWORD* namePointer = (DWORD*)(data + namePos);
    for (auto&& pair : modules)
    {
        auto m = pair.second;
        for (auto&& e : m->externalNames)
        {
            auto name = std::get<0>(e);
            auto sym = std::get<1>(e);
            if (name.empty())
            {
                // byordinal
                *iatPointer = *namePointer = std::get<2>(e) | IMPORT_BY_ORDINAL;
            }
            else
            {
                // by name
                *iatPointer = *namePointer = RVA(stringPos);
            }
            std::copy(name.begin(), name.end(), data + stringPos + sizeof(HintType));
            sym->SetOffset(new ObjExpression(RVA((iatPointer - iatBase) * sizeof(DWORD) + iatPos) + imageBase));
            iatPointer++;
            namePointer++;
            int sz = 1 + sizeof(HintType) + name.size();
            sz += (sz & 1);
            stringPos += sz;
        }
        iatPointer++;
        namePointer++;
    }
}

template <>
void DllImports<DelayLoadDirectory>::TrimModules(const std::vector<std::string>& delayLoadNames)
{
    for (auto it = modules.begin(); it != modules.end();)
    {
        if (!IsDelayLoad(delayLoadNames, it->second->name))
        {
            it = modules.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
template <>
void DllImports<DelayLoadDirectory>::WriteDirectory(DWORD virtual_addr, DWORD imageBase, unsigned char* data)
{
    DWORD directoryPos = directory;
    DWORD iatPos = iatAddr;
    DWORD namePos = nameAddr;
    DWORD stringPos = directoryNames;
    DWORD bindPos = bindAddr ? bindAddr : 0;
    DWORD unloadPos = unloadAddr ? unloadAddr : 0;
    for (auto&& pair : modules)
    {
        auto m = pair.second;
        DelayLoadDirectory dir = {DllImports::DELAYLOAD_BY_RVA,
                                  RVA(stringPos),
                                  m->moduleHandleRVA,
                                  RVA(iatPos),
                                  RVA(namePos),
#ifdef TARGET_OS_WINDOWS
                                  bindPos ? RVA(bindPos) : 0,
#else
                                  0,
#endif
                                  unloadPos ? RVA(unloadPos) : 0,
#ifdef TARGET_OS_WINDOWS
                                  m->time
#else
                                  0
#endif
        };

        std::copy((char*)&dir, (char*)(&dir + 1), data + directoryPos);
        std::copy(m->name.begin(), m->name.end(), data + stringPos);
        directoryPos += sizeof(DelayLoadDirectory);
        iatPos += (m->externalNames.size() + 1) * sizeof(DWORD);
        namePos += (m->externalNames.size() + 1) * sizeof(DWORD);
        int sz = m->name.size() + 1;
        sz += (sz & 1);
        stringPos += sz;
        if (bindPos)
            bindPos += (m->externalNames.size() + 1) * sizeof(DWORD);
        if (unloadPos)
            unloadPos += (m->externalNames.size() + 1) * sizeof(DWORD);
    }
}
template <>
void DllImports<DelayLoadDirectory>::WriteTables(std::vector<DWORD>& thunkFixups, DWORD virtual_addr, DWORD imageBase,
                                                 DWORD& thunkTableRVA, unsigned char* data)
{
    DWORD iatPos = iatAddr;
    DWORD namePos = nameAddr;
    DWORD stringPos = names;
    DWORD bindPos = bindAddr ? bindAddr : 0;
    DWORD unloadPos = unloadAddr ? unloadAddr : 0;
    DWORD *iatPointer = (DWORD*)(data + iatPos), *iatBase = iatPointer;
    DWORD* namePointer = (DWORD*)(data + namePos);
    DWORD* bindPointer = (DWORD*)(data + bindPos);
    DWORD* unloadPointer = (DWORD*)(data + unloadPos);
    for (auto&& pair : modules)
    {
        auto m = pair.second;
        for (auto&& e : m->externalNames)
        {
            auto name = std::get<0>(e);
            auto sym = std::get<1>(e);
            if (name.empty())
            {
                // byordinal
                *namePointer = std::get<2>(e) | IMPORT_BY_ORDINAL;
            }
            else
            {
                // by name
                *namePointer = RVA(stringPos);
            }
            *iatPointer = thunkTableRVA + imageBase;
            thunkFixups.push_back(RVA((BYTE*)iatPointer - data) + imageBase);
            if (unloadPos)
            {
                *unloadPointer = thunkTableRVA + imageBase;
                thunkFixups.push_back(RVA((BYTE*)unloadPointer - data) + imageBase);
            }
#ifdef TARGET_OS_WINDOWS
            if (bindPos)
            {
                *bindPointer = (DWORD)(unsigned long long)std::get<3>(e);
            }
#endif
            thunkTableRVA += PEImportObject::DelayLoadThunkSize;
            std::copy(name.begin(), name.end(), data + stringPos + sizeof(HintType));
            sym->SetOffset(new ObjExpression(RVA((iatPointer - iatBase) * sizeof(DWORD) + iatPos) + imageBase));
            iatPointer++;
            namePointer++;
            bindPointer++;
            unloadPointer++;
            int sz = 1 + sizeof(HintType) + name.size();
            sz += (sz & 1);
            stringPos += sz;
        }
        iatPointer++;
        namePointer++;
        bindPointer++;
        unloadPointer++;
    }
}

PEImportObject::PEImportObject(std::deque<std::shared_ptr<PEObject>>& Objects, bool BindTable, bool UnloadTable) :
    PEObject(".rdata"), objects(Objects), bindTable(BindTable), unloadTable(UnloadTable)
{
    SetFlags(WINF_INITDATA | WINF_READABLE | WINF_WRITEABLE | WINF_NEG_FLAGS);
}
void PEImportObject::LoadHandles(DllImports<DelayLoadDirectory>& delay)
{
    for (auto&& m : delay.Modules())
    {
        m.second->moduleHandleRVA = delayLoadHandleRVA;
        delayLoadHandleRVA += sizeof(DWORD);
    }
}
void PEImportObject::LoadBindingInfo(DllImports<DelayLoadDirectory>& delay, std::map<std::string, Module*> modules)
{
#ifdef TARGET_OS_WINDOWS
    if (bindTable)
    {
        for (auto&& m : modules)
        {
            HMODULE handle = LoadLibrary(m.second->name.c_str());
            if (handle != nullptr)
            {
                bool complete = true;
                for (auto&& e : m.second->externalNames)
                {
                    FARPROC entry = GetProcAddress(handle, std::get<0>(e).c_str());
                    if (entry)
                    {
                        std::get<3>(e) = entry;
                    }
                    else
                    {
                        complete = false;
                    }
                }
                if (complete)
                {
                    PEHeader* hdr = reinterpret_cast<PEHeader*>(*(DWORD*)((char*)handle + 0x3c) + (char*)handle);
                    m.second->time = hdr->time;
                }
                FreeLibrary(handle);
            }
        }
    }
#endif
}
size_t PEImportObject::ThunkSize(std::string name)
{
    size_t rv = 0;
    if (delayLoadNames.size())
    {
        std::set<std::string> dllNames;
        for (auto&& d : delayLoadNames)
        {
            size_t n = d.find_last_of('/');
            size_t m = d.find_last_of('\\');
            if (n > m)
                n = m;
            ;
            if (n == std::string::npos)
                n = 0;

            dllNames.insert(d.substr(n));
        }
        if (name == ".text")
        {
            std::map<std::string, ObjSymbol*> externs;
            for (auto it = file->ExternalBegin(); it != file->ExternalEnd(); ++it)
            {
                externs[(*it)->GetName()] = (*it);
            }
            rv = delayLoadNames.size() * PEImportObject::DelayLoadModuleThunkSize;
            for (auto it = file->ImportBegin(); it != file->ImportEnd(); ++it)
            {
                ObjImportSymbol* s = (ObjImportSymbol*)(*it);
                // uppercase the module name for NT... 98 doesn't need it but can accept it
                std::string name = s->GetDllName();
                std::transform(name.begin(), name.end(), name.begin(), ::toupper);
                if (dllNames.find(name) != dllNames.end() && externs.find(s->GetName()) != externs.end())
                {
                    rv += PEImportObject::DelayLoadThunkSize;
                }
            }
        }
        else if (name == ".data")
        {
            rv = delayLoadNames.size() * sizeof(DWORD);
        }
    }
    return rv;
}

void PEImportObject::SymbolNotFoundError()
{
    Utils::Fatal("Could not find delay load symbols, try adding delayimp.l to the command line");
}
void PEImportObject::WriteThunks(DllImports<DelayLoadDirectory>& delay, std::map<std::string, Module*> modules,
                                 std::map<ObjString, ObjSymbol*> publics)
{
    auto it = publics.find("___delayLoadHelper2");
    if (it == publics.end())
        SymbolNotFoundError();
    auto helperAddr = it->second->GetOffset()->Eval(0);

    auto entryThunkRVA = delayLoadThunkRVA;
    auto moduleThunkRVA = delayLoadThunkRVA + (delay.CountOfIAT() - delay.Modules().size()) * DelayLoadThunkSize;
    DWORD iatPos = delay.IATOffset();
    for (auto&& m : delay.Modules())
    {
        for (auto&& e : m.second->externalNames)
        {
            unsigned char* thunkPtr = codeData.get() + entryThunkRVA - codeRVA;
            memcpy(thunkPtr, delayLoadThunk, PEImportObject::DelayLoadThunkSize);
            *(DWORD*)(thunkPtr + 1) = RVA(iatPos) + imageBase;
            thunkFixups.push_back(entryThunkRVA + 1 + imageBase);
            *(DWORD*)(thunkPtr + 6) = moduleThunkRVA - (entryThunkRVA + 6 + 4);
            // make one fixup to the iat
            entryThunkRVA += PEImportObject::DelayLoadThunkSize;
            iatPos += sizeof(DWORD);
        }
        moduleThunkRVA += PEImportObject::DelayLoadModuleThunkSize;
        iatPos += sizeof(DWORD);
    }
    DWORD descriptPos = delay.DirectoryOffset();
    moduleThunkRVA = delayLoadThunkRVA + (delay.CountOfIAT() - delay.Modules().size()) * DelayLoadThunkSize;
    for (auto&& m : delay.Modules())
    {
        unsigned char* thunkPtr = codeData.get() + moduleThunkRVA - codeRVA;
        memcpy(thunkPtr, delayLoadModuleThunk, PEImportObject::DelayLoadModuleThunkSize);
        *(DWORD*)(thunkPtr + 4) = RVA(descriptPos) + imageBase;
        thunkFixups.push_back(moduleThunkRVA + 4 + imageBase);
        *(DWORD*)(thunkPtr + 4 + 5) = (helperAddr - imageBase) - (moduleThunkRVA + 4 + 5 + 4);
        moduleThunkRVA += PEImportObject::DelayLoadModuleThunkSize;
        descriptPos += sizeof(DelayLoadDirectory);
    }
}
void PEImportObject::Setup(ObjInt& endVa, ObjInt& endPhys)
{
    if (virtual_addr == 0)
    {
        virtual_addr = endVa;
    }
    else
    {
        if (virtual_addr != endVa)
            Utils::Fatal("Internal error");
    }
    size = 0;
    raw_addr = endPhys;
    std::map<ObjString, ObjSymbol*> externs;
    for (auto it = file->ExternalBegin(); it != file->ExternalEnd(); ++it)
    {
        externs[(*it)->GetName()] = (*it);
    }
    std::map<std::string, Module*> modules;
    for (auto it = file->ImportBegin(); it != file->ImportEnd(); ++it)
    {
        ObjImportSymbol* s = (ObjImportSymbol*)(*it);
        // uppercase the module name for NT... 98 doesn't need it but can accept it
        std::string name = s->GetDllName();
        std::transform(name.begin(), name.end(), name.begin(), ::toupper);
        s->SetDllName(name);
        auto it1 = externs.find((*it)->GetName());
        if (it1 != externs.end())
        {
            int sz;
            Module* m = modules[s->GetDllName()];
            if (m == nullptr)
            {
                modules[s->GetDllName()] = m = new Module;
                m->name = std::move(name);
            }
            if (s->GetExternalName().size() == 0)
            {
                m->externalNames.push_back(std::tuple<std::string, ObjSymbol*, DWORD, FARPROC>(
                    s->GetName(), it1->second, s->GetByOrdinal() ? s->GetOrdinal() : 0xffffffff, nullptr));
            }
            else
            {
                m->externalNames.push_back(std::tuple<std::string, ObjSymbol*, DWORD, FARPROC>(
                    s->GetExternalName(), it1->second, s->GetByOrdinal() ? s->GetOrdinal() : 0xffffffff, nullptr));
            }
            m->publicNames.push_back(s->GetName());
        }
    }

    DllImports<ImportDirectory> imports(modules, delayLoadNames, 0);
    int importSize = imports.ModuleSize(externs);
    DllImports<DelayLoadDirectory> delayLoad(modules, delayLoadNames, importSize, bindTable, unloadTable);
    if (delayLoad.Modules().size())
    {
        LoadHandles(delayLoad);
        LoadBindingInfo(delayLoad, delayLoad.Modules());
    }
    int delayLoadImportSize = delayLoad.ModuleSize(std::move(externs));
    initSize = size = importSize + delayLoadImportSize;
    data = std::shared_ptr<unsigned char>(new unsigned char[size]);
    std::fill(data.get(), data.get() + size, 0);
    imports.WriteDirectory(virtual_addr, imageBase, data.get());
    DWORD thunk = delayLoadThunkRVA;
    imports.WriteTables(thunkFixups, virtual_addr, imageBase, thunk, data.get());
    delayLoad.WriteDirectory(virtual_addr, imageBase, data.get());
    thunk = delayLoadThunkRVA;
    delayLoad.WriteTables(thunkFixups, virtual_addr, imageBase, thunk, data.get());

    if (delayLoad.Modules().size())
    {
        std::map<ObjString, ObjSymbol*> publics;
        for (auto it = file->PublicBegin(); it != file->PublicEnd(); ++it)
        {
            publics[(*it)->GetName()] = (*it);
        }

        WriteThunks(delayLoad, std::move(modules),std::move( publics));
    }
    regions.push_back(std::pair<ObjInt, ObjInt>(virtual_addr, importSize));
    if (delayLoadImportSize)
        regions.push_back(std::pair<ObjInt, ObjInt>(virtual_addr + importSize, delayLoadImportSize));
    else
        regions.push_back(std::pair<ObjInt, ObjInt>(0, 0));
    endVa = ObjectAlign(objectAlign, endVa + size);
    endPhys = ObjectAlign(fileAlign, endPhys + initSize);
}
