/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

#include "Utils.h"
#include "CoffLibrary.h"
#include "CoffFile.h"
#include "ObjIeee.h"
#include "ObjFactory.h"
#include <cstdio>
#include <cstdlib>

CoffLibrary::~CoffLibrary() {}
bool CoffLibrary::Load()
{
    inputFile = std::make_unique<std::fstream>(name, std::ios::in | std::ios::binary);
    if (inputFile && inputFile->is_open())
    {
        inputFile->seekg(
            8);  // point to first linker member, we are going to ignore the second linker member and the longnames member
        return LoadNames();
    }
    else
    {
        inputFile.release();
    }
    return inputFile != nullptr;
}
bool CoffLibrary::Convert() { return ScanIntegrity() && ConvertNormalMods() && ConvertImportMods(); }
bool CoffLibrary::Write(std::string fileName)
{
    for (auto& m : modules)
    {
        if (!m.second->ignore && !m.second->import)
        {
            files.Add(*(m.second->file));
        }
    }
    if (importFile)
        files.Add(*importFile);
    return SaveLibrary(fileName);
}

#define swap(x) (((x & 0xff) << 24) | ((x & 0xff00) << 8) | ((x & 0xff0000) >> 8) | ((x & 0xff000000) >> 24))

bool CoffLibrary::LoadNames()
{
    CoffLinkerMemberHeader firstHeader;
    inputFile->read((char*)&firstHeader, sizeof(firstHeader));
    if (!memcmp(firstHeader.EndOfHeader, "`\n", 2))
    {
        firstHeader.EndOfHeader[0] = 0;
        int size = atoi(firstHeader.Size);
        std::unique_ptr<ObjByte[]> firstMemberData = std::make_unique<ObjByte[]>(size);
        inputFile->read((char*)firstMemberData.get(), size);
        if (!inputFile->fail())
        {
            CoffLinkerSectionOneHeader* hdr = (CoffLinkerSectionOneHeader*)firstMemberData.get();
            int n = swap(hdr->NumberOfSymbols);
            char* strings = (char*)&hdr->OffsetArray[n];
            for (int i = 0; i < n; i++, strings += strlen(strings) + 1)
            {
                Module* m;
                int t = swap(hdr->OffsetArray[i]);
                auto it = modules.find(t);
                if (it == modules.end())
                {
                    modules[t] = std::make_unique<Module>();
                    modules[t]->fileOffset = t;
                    m = modules[t].get();
                }
                else
                {
                    m = it->second.get();
                }
                if (!strncmp(strings, "__imp", 5))
                    strings += 5;
                m->aliases.insert(strings);
            }
        }
        return !inputFile->fail();
    }
    std::cout << "Invalid first linker member" << std::endl;
    return false;
}
bool CoffLibrary::LoadHeaders()
{
    for (auto& m : modules)
    {
        inputFile->seekg(m.first);
        inputFile->read((char*)&m.second->header, sizeof(m.second->header));
        if (!memcmp(m.second->header.EndOfHeader, "`\n", 2))
        {
            m.second->header.EndOfHeader[0] = 0;
        }
        else
        {
            std::cout << "Invalid linker member header" << std::endl;
            return false;
        }
    }
    return true;
}
bool CoffLibrary::ScanIntegrity()
{
    for (auto& module : modules)
    {
        inputFile->seekg(module.first + sizeof(CoffLinkerMemberHeader));
        ObjByte buf[8];
        inputFile->read((char*)buf, 8);
        if (inputFile->fail())
            return false;
        module.second->ignore = false;
        if (*(unsigned short*)buf == IMAGE_FILE_MACHINE_I386)
        {
            module.second->import = false;
        }
        else
        {
            bool found = false;
            if (*(unsigned short*)buf == IMAGE_FILE_MACHINE_UNKNOWN)
                if (*(unsigned short*)(buf + 2) == 0xffff)
                    //                   if (*(unsigned short *)(buf+6) == IMAGE_FILE_MACHINE_I386)
                    module.second->import = found = true;
            if (!found)
            {
                module.second->ignore = true;
            }
        }
    }
    return true;
}
bool CoffLibrary::ConvertNormalMods()
{
    for (auto& m : modules)
    {
        if (!m.second->import && !m.second->ignore)
        {
            if (m.second->aliases.begin()->find("_IMPORT_DESCRIPTOR") == std::string::npos &&
                m.second->aliases.begin()->find("_nullptr_THUNK_DATA") == std::string::npos)
            {
                CoffFile object(inputFile.get(), m.first + sizeof(CoffLinkerMemberHeader));
                inputFile->seekg(m.first + sizeof(CoffLinkerMemberHeader));
                if (object.Load())
                {
                    char name[256];
                    sprintf(name, "%d.o", m.first);
                    ObjFile* file = object.ConvertToObject(name, m.second->factory);
                    if (file)
                    {
                        m.second->file = file;
                    }
                    else
                    {
                        std::cout << "Could not translate object member" << std::endl;
                        return false;
                    }
                }
                else
                {
                    std::cout << "Could not load object member" << std::endl;
                    return false;
                }
            }
            else
            {
                m.second->ignore = true;
            }
        }
    }
    return true;
}
bool CoffLibrary::ConvertImportMods()
{
    bool foundImports = false;
    for (auto& m : modules)
    {
        if (m.second->import && !m.second->ignore)
        {
            foundImports = true;
            inputFile->seekg(m.first + sizeof(CoffLinkerMemberHeader));
            CoffImportHeader hdr;
            inputFile->read((char*)&hdr, sizeof(hdr));
            if (inputFile->fail())
                return false;
            char buf[512], *p = buf;
            inputFile->read(buf, hdr.Size);
            p = buf + strlen(buf) + 1;
            switch (hdr.ImportNameType)
            {
                case 0:
                    Utils::Fatal("Can't import by ordinal");
                    exit(1);
                    break;
                case 1:  // identical to public
                    break;
                case 2:  // strip leading char
                    if (buf[0] == '@' || buf[0] == '?' || buf[0] == '_')
                        memmove(buf, buf + 1, strlen(buf + 1) + 1);
                    break;
                case 3:  // undecorate
                {
                    char* p = (char*)strchr(buf, '@');
                    if (p)
                        *p = 0;
                    if (buf[0] == '@' || buf[0] == '?' || buf[0] == '_')
                        memmove(buf, buf + 1, strlen(buf + 1) + 1);
                }
                break;
            }
            m.second->importName = buf;
            m.second->importDLL = p;
        }
    }
    if (foundImports)
    {
        ObjFile* file = new ObjFile("__imports__.o");
        for (auto& m : modules)
        {
            if (m.second->import && !m.second->ignore)
            {
                for (auto alias : m.second->aliases)
                {
                    ObjImportSymbol* sym = importFactory.MakeImportSymbol(alias);
                    sym->SetExternalName(m.second->importName);
                    sym->SetDllName(m.second->importDLL);
                    file->Add(sym);
                }
            }
        }
        importFile = file;
    }
    return true;
}
bool CoffLibrary::SaveLibrary(std::string name)
{
    memset(&header, 0, sizeof(header));
    header.sig = LibManager::LibHeader::LIB_SIG;
    dictionary.CreateDictionary(modules);
    // can't do more reading
    header.filesInModule = files.size();
    FILE* ostr = fopen(name.c_str(), "wb");
    if (!ostr)
        Utils::Fatal("Cannot open '%s' for write", name.c_str());
    fwrite(&header, sizeof(header), 1, ostr);
    Align(ostr, 16);
    header.namesOffset = ftell(ostr);
    files.WriteNames(ostr);
    Align(ostr);
    header.filesOffset = ftell(ostr);
    files.WriteFiles(ostr, LibManager::ALIGN);
    Align(ostr);
    header.offsetsOffset = ftell(ostr);
    files.WriteOffsets(ostr);
    Align(ostr);
    header.dictionaryOffset = ftell(ostr);
    header.dictionaryBlocks = 0;
    dictionary.Write(ostr);
    fseek(ostr, 0, SEEK_SET);
    fwrite(&header, sizeof(header), 1, ostr);
    return true;
    //    return !ostr.fail();
}
void CoffLibrary::Align(FILE* ostr, ObjInt align)
{
    char buf[LibManager::ALIGN];
    memset(buf, 0, align);
    int n = ftell(ostr);
    if (n % align)
        fwrite(buf, align - (n % align), 1, ostr);
}
