/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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

#include "LibManager.h"
#include "LibFiles.h"
#include "ObjFile.h"
#include "ObjIeee.h"
#include "ObjFactory.h"
#include "CmdFiles.h"
#include <iostream>
#include <cstring>

void LibFiles::Add(ObjFile& obj)
{
    for (size_t i = 0; i < files.size(); i++)
        if (files[i]->name == obj.GetName())
        {
            std::cout << "Warning: module '" << files[i]->name << "' already exists in library, it won't be added"
                      << std::endl;
            return;
        }
    files.push_back(std::make_unique<FileDescriptor>(obj.GetName()));
    files.back()->data = &obj;
}
void LibFiles::Add(const ObjString& Name)
{
    unsigned npos = Name.find_last_of(CmdFiles::DIR_SEP);
    unsigned npos1 = Name.find_last_of('/');
    if (npos != std::string::npos && npos1 != std::string::npos && npos1 > npos)
        npos = npos1;
    std::string internalName = Name;
    if (npos != std::string::npos)
        internalName = Name.substr(npos + 1);
    for (size_t i = 0; i < files.size(); i++)
        if (files[i]->name == internalName)
        {
            std::cout << "Warning: module '" << Name << "' already exists in library, it won't be added" << std::endl;
            return;
        }
    files.push_back(std::make_unique<FileDescriptor>(Name));
}
void LibFiles::Remove(const ObjString& Name)
{
    unsigned npos = Name.find_last_of(CmdFiles::DIR_SEP);
    std::string internalName = Name;
    if (npos != std::string::npos)
        internalName = Name.substr(npos + 1);
    for (auto it = FileBegin(); it != FileEnd(); ++it)
    {
        if ((*it)->name == internalName)
        {
            files.erase(it);
            return;
        }
    }
    std::cout << "Warning: Module '" << Name << "' not in library and could not be removed" << std::endl;
}
void LibFiles::Extract(FILE* stream, const ObjString& Name)
{
    unsigned npos = Name.find_last_of(CmdFiles::DIR_SEP);
    std::string internalName = Name;
    if (npos != std::string::npos)
        internalName = Name.substr(npos + 1);
    int count = 0;
    ObjIeeeIndexManager im1;
    ObjFactory fact1(&im1);
    for (auto it = FileBegin(); it != FileEnd(); ++it)
    {
        if ((*it)->name == internalName)
        {
            ObjFile* p = LoadModule(stream, count, &fact1);
            if (p)
            {
                FILE* ostr = fopen(Name.c_str(), "wb");
                if (ostr != nullptr)
                {
                    WriteData(ostr, p, (*it)->name);
                    fclose(ostr);
                }
                else
                {
                    std::cout << "Warning: Module '" << Name << "' not extracted, could not open output file" << std::endl;
                }
            }
            else
            {
                std::cout << "Warning: Module '" << Name << "' not extracted, library corrupt" << std::endl;
            }
            return;
        }
    }
    std::cout << "Warning: Module '" << Name << "' not in library and could not be extracted" << std::endl;
}
void LibFiles::Replace(ObjFile& obj)
{
    std::string Name = obj.GetName();
    unsigned npos = Name.find_last_of(CmdFiles::DIR_SEP);
    std::string internalName = Name;
    if (npos != std::string::npos)
        internalName = Name.substr(npos + 1);
    for (auto it = FileBegin(); it != FileEnd(); ++it)
    {
        if ((*it)->name == internalName)
        {
            if ((*it)->data)
            {
                (*it)->data = nullptr;
            }
            (*it)->data = &obj;
            (*it)->name = Name;
            (*it)->offset = 0;
            return;
        }
    }
    Add(obj);
}
void LibFiles::Replace(const ObjString& Name)
{
    unsigned npos = Name.find_last_of(CmdFiles::DIR_SEP);
    std::string internalName = Name;
    if (npos != std::string::npos)
        internalName = Name.substr(npos + 1);
    for (auto it = FileBegin(); it != FileEnd(); ++it)
    {
        if ((*it)->name == internalName)
        {
            if ((*it)->data)
            {
                (*it)->data = nullptr;
            }
            (*it)->name = Name;
            (*it)->offset = 0;
            return;
        }
    }
    Add(Name);
}
void LibFiles::WriteData(FILE* stream, ObjFile* file, const ObjString& name)
{
    ObjIeeeIndexManager im1;
    ObjFactory fact1(&im1);
    ObjIeee ieee(name.c_str());
    ieee.Write(stream, file, &fact1);
}
void LibFiles::WriteNames(FILE* stream)
{
    for (auto it = FileBegin(); it != FileEnd(); ++it)
    {
        const char* p = (*it)->name.c_str();
        const char* q = strrchr(p, '\\');
        const char* q1 = strrchr(p, '/');
        if (q && q1)
            q = q > q1 ? q : q1;
        else if (q == 0)
            q = q1;
        if (q)
            q++;
        else
            q = p;
        fwrite(q, strlen(q) + 1, 1, stream);
    }
}
void LibFiles::WriteOffsets(FILE* stream)
{
    for (auto it = FileBegin(); it != FileEnd(); ++it)
    {
        ObjInt ofs = (*it)->offset;
        fwrite(&ofs, 4, 1, stream);
    }
}
void LibFiles::Align(FILE* stream, int align)
{
    char buf[LibManager::ALIGN];
    memset(buf, 0, align);
    int n = ftell(stream);
    if (n % align)
        fwrite(buf, align - n % align, 1, stream);
}
bool LibFiles::ReadFiles(FILE* stream, ObjFactory* factory)
{
    bool rv = true;
    bool done = false;
    while (!done)
    {
        done = true;
        for (auto it = FileBegin(); it != FileEnd();)
        {
            auto itn = it;
            ++it;
            if (!(*itn)->data)
            {
                if ((*itn)->offset)
                {
                    fseek(stream, (*itn)->offset, SEEK_SET);
                    (*itn)->data = ReadData(stream, (*itn)->name, factory);
                    if (!(*itn)->data)
                    {
                        std::cout << "Error: Syntax error in module '" << (*itn)->name << "'" << std::endl;
                        files.erase(itn);
                        done = false;
                        rv = false;
                        break;
                    }
                }
                else
                {
                    FILE* istr = fopen((*itn)->name.c_str(), "rb");
                    if (istr != nullptr)
                    {
                        (*itn)->data = ReadData(istr, (*itn)->name, factory);
                        fclose(istr);
                        if (!(*itn)->data)
                        {
                            std::cout << "Error: Syntax error in module '" << (*itn)->name << "'" << std::endl;
                            files.erase(itn);
                            done = false;
                            rv = false;
                            break;
                        }
                    }
                    else
                    {
                        std::cout << "Error: Module '" << (*itn)->name << "' does not exist" << std::endl;
                        files.erase(itn);
                        done = false;
                        rv = false;
                        break;
                    }
                }
            }
        }
    }
    return rv;
}
void LibFiles::WriteFiles(FILE* stream, ObjInt align)
{
    int i = 0;
    for (auto it = FileBegin(); it != FileEnd(); ++it)
    {
        Align(stream, align);
        (*it)->offset = ftell(stream);
        WriteData(stream, (*it)->data, (*it)->name);
    }
}
