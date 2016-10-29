/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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
#include "CoffLibrary.h"
#include "CoffFile.h"
#include "ObjIeee.h"
#include "ObjFactory.h"
#include <stdio.h>
#include <stdlib.h>

CoffLibrary::~CoffLibrary()
{
    for (std::map<int, Module *>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        Module *m = it->second;
        delete m;
    }
    if (inputFile)
        delete inputFile;
}
bool CoffLibrary::Load()
{
    inputFile = new std::fstream(name.c_str(), std::ios::in | std::ios::binary);
    if (inputFile && inputFile->is_open())
    {
        inputFile->seekg(8); // point to first linker member, we are going to ignore the second linker member and the longnames member
        return LoadNames();
                
    }
    else
    {
        delete inputFile;
        inputFile = NULL;
    }
    return inputFile != NULL;
}
bool CoffLibrary::Convert()
{
    return ScanIntegrity() && ConvertNormalMods() && ConvertImportMods();
}
bool CoffLibrary::Write(std::string fileName)
{
    for (std::map<int, Module *>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        if (!it->second->ignore && !it->second->import)
        {
            files.Add(*(it->second->file));
        }
    }
    if (importFile)
        files.Add(*importFile);
    return SaveLibrary(fileName);
}

#define swap(x) (((x&0xff)<<24) | ((x & 0xff00) << 8) | (( x & 0xff0000) >> 8) | ((x & 0xff000000) >> 24))

bool CoffLibrary::LoadNames()
{
    CoffLinkerMemberHeader firstHeader;
    inputFile->read((char *)&firstHeader, sizeof(firstHeader));
    if (!memcmp(firstHeader.EndOfHeader, "`\n", 2))
    {
        firstHeader.EndOfHeader[0] = 0;
        int size = atoi(firstHeader.Size);
        ObjByte *firstMemberData = new ObjByte[size];
        inputFile->read((char *)firstMemberData, size);
        if (!inputFile->fail())
        {
            CoffLinkerSectionOneHeader *hdr = (CoffLinkerSectionOneHeader *)firstMemberData;
            int n = swap(hdr->NumberOfSymbols);
            char *strings = (char *)&hdr->OffsetArray[n];
            for (int i=0; i < n; i++, strings += strlen(strings)+1)
            {
                Module *m;
                int t = swap(hdr->OffsetArray[i]);
                std::map<int, Module *>::iterator it = modules.find(t);
                if (it == modules.end())
                {
                    m = new Module;
                    m->fileOffset = t;
                    modules[t] = m;
                }
                else
                {
                    m = it->second;
                }
                m->aliases.insert(strings);
            }
        }
        delete [] firstMemberData;
        return !inputFile->fail();
    }
    std::cout << "Invalid first linker member" << std::endl;
    return false;
}
bool CoffLibrary::LoadHeaders()
{
    for (std::map<int, Module *>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        Module *m = it->second;
        inputFile->seekg(it->first);
        inputFile->read((char *)&m->header, sizeof(m->header));
        if (!memcmp(m->header.EndOfHeader, "`\n", 2))
        {
            m->header.EndOfHeader[0] = 0;
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
    for (std::map<int, Module *>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        inputFile->seekg(it->first + sizeof(CoffLinkerMemberHeader));
        ObjByte buf[8];
        inputFile->read((char *)buf, 8);
        if (inputFile->fail())
            return false;
		it->second->ignore = false;
        if (*(unsigned short *)buf == IMAGE_FILE_MACHINE_I386)
        {
            it->second->import = false;
        }
        else
        {
            bool found = false;
            if (*(unsigned short *)buf ==IMAGE_FILE_MACHINE_UNKNOWN)
               if (*(unsigned short *)(buf+2) == 0xffff)
                   if (*(unsigned short *)(buf+6) == IMAGE_FILE_MACHINE_I386)
                       it->second->import = found = true;
            if (!found)
            {
                it->second->ignore = true;
            }
        }            
    }
    return true;
}
bool CoffLibrary::ConvertNormalMods()
{
    for (std::map<int, Module *>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        if (!it->second->import && !it->second->ignore)
        {
            if (it->second->aliases.begin()->find("_IMPORT_DESCRIPTOR") == std::string::npos &&
                it->second->aliases.begin()->find("_NULL_THUNK_DATA") == std::string::npos)
            {
                CoffFile object(inputFile, it->first + sizeof(CoffLinkerMemberHeader));
                inputFile->seekg(it->first + sizeof(CoffLinkerMemberHeader));
                if (object.Load())
                {
                    char name[256];
                    sprintf(name, "%d.o", it->first);
                    ObjFile *file = object.ConvertToObject(name, it->second->factory);
                    if (file)
                    {
                        it->second->file = file;
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
				it->second->ignore = true;
			}
        }
    }
    return true;
}
bool CoffLibrary::ConvertImportMods()
{
    bool foundImports = false;
    for (std::map<int, Module *>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        if (it->second->import && !it->second->ignore)
        {
            foundImports = true;
            inputFile->seekg(it->first + sizeof(CoffLinkerMemberHeader));
            CoffImportHeader hdr;
            inputFile->read((char *)&hdr, sizeof(hdr));
            if (inputFile->fail())
                return false;
            char buf[512], *p = buf;
            inputFile->read(buf, hdr.Size);
            p = buf + strlen(buf)+1;
            switch(hdr.ImportNameType)
            {
                case 0:
                    Utils::fatal("Can't import by ordinal");
                    exit(1);
                    break;
                case 1: // identical to public
                    break;
                case 2: // strip leading char
                    if (buf[0] == '@' || buf[0] == '?' || buf[0] == '_')
                        strcpy(buf, buf+1);
                    break;
                case 3: // undecorate
                    {
                        char *p = strchr(buf, '@');
                        if (p)
                            *p = 0;
                        if (buf[0] == '@' || buf[0] == '?' || buf[0] == '_')
                            strcpy(buf, buf+1);
                    } 
                    break;
            }
            it->second->importName = buf;
            it->second->importDLL = p;
        }
    }
    if (foundImports)
    {
        ObjFile *file = new ObjFile("__imports__.o");
        for (std::map<int, Module *>::iterator it = modules.begin(); it != modules.end(); ++it)
        {
            if (it->second->import && !it->second->ignore)
            {
                for (std::set<std::string>::iterator it1 = it->second->aliases.begin(); it1 != it->second->aliases.end(); ++it1)
                {
                    ObjImportSymbol *sym = importFactory.MakeImportSymbol(*it1);
                    sym->SetExternalName(it->second->importName);
                    sym->SetDllName(it->second->importDLL);
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
    FILE * ostr = fopen(name.c_str(), "wb");
    fwrite(&header, sizeof(header), 1, ostr);
    Align(ostr,16);
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
void CoffLibrary::Align(FILE *ostr, ObjInt align)
{
    char buf[LibManager::ALIGN];
    memset(buf, 0, align);
    int n = ftell(ostr);
    if ( n % align)
        fwrite(buf, align - (n % align), 1, ostr);
}
