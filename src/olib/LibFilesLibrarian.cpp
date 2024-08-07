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

#include "LibManager.h"
#include "LibFiles.h"
#include "ObjFile.h"
#include "ObjIeee.h"
#include "ObjFactory.h"
#include "CmdFiles.h"
#include <iostream>
#include <cstring>
#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#else
#    include <io.h>
#endif

void LibFiles::Add(ObjFile& obj)
{
    for (int i = 0; i < files.size(); i++)
        if (files[i]->name == obj.GetName())
        {
            std::cout << "Warning: module '" << files[i]->name << "' already exists in library, it won't be added" << std::endl;
            return;
        }
    files.push_back(std::make_unique<FileDescriptor>(obj.GetName()));
    files.back()->data = &obj;
}
void LibFiles::Add(const ObjString& Name)
{
    size_t npos = Name.find_last_of(CmdFiles::DIR_SEP);
    size_t npos1 = Name.find_last_of('/');
    if (npos != std::string::npos && npos1 != std::string::npos && npos1 > npos)
        npos = npos1;
    std::string internalName = Name;
    if (npos != std::string::npos)
        internalName = Name.substr(npos + 1);
    for (int i = 0; i < files.size(); i++)
        if (files[i]->name == internalName)
        {
            std::cout << "Warning: module '" << Name << "' already exists in library, it won't be added" << std::endl;
            return;
        }
    files.push_back(std::make_unique<FileDescriptor>(Name));
}
void LibFiles::Remove(const ObjString& Name)
{
    size_t npos = Name.find_last_of(CmdFiles::DIR_SEP);
    std::string internalName = Name;
    if (npos != std::string::npos)
        internalName = Name.substr(npos + 1);
    for (auto it = begin(); it != end(); ++it)
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
    size_t npos = Name.find_last_of(CmdFiles::DIR_SEP);
    std::string internalName = Name;
    if (npos != std::string::npos)
        internalName = Name.substr(npos + 1);
    int count = 0;
    ObjIeeeIndexManager im1;
    ObjFactory fact1(&im1);
    for (auto&& file : *this)
    {
        if (file->name == internalName)
        {
            ObjFile* p = LoadModule(stream, count, &fact1);
            if (p)
            {
                FILE* ostr = fopen(Name.c_str(), "wb");
                if (ostr != nullptr)
                {
                    if (!WriteData(ostr, p, file->name))
                    {

                        std::cout << "Warning: Module '" << Name << "' not extracted, could not write output file" << std::endl;
                        fclose(ostr);
                        unlink(Name.c_str());
                    }
                    else
                    {
                        fclose(ostr);
                    }
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
    size_t npos = Name.find_last_of(CmdFiles::DIR_SEP);
    std::string internalName = Name;
    if (npos != std::string::npos)
        internalName = Name.substr(npos + 1);
    for (auto&& file : *this)
    {
        if (file->name == internalName)
        {
            if (file->data)
            {
                file->data = nullptr;
            }
            file->data = &obj;
            file->name = Name;
            file->offset = 0;
            return;
        }
    }
    Add(obj);
}
void LibFiles::Replace(const ObjString& Name)
{
    size_t npos = Name.find_last_of(CmdFiles::DIR_SEP);
    std::string internalName = Name;
    if (npos != std::string::npos)
        internalName = Name.substr(npos + 1);
    for (auto&& file : *this)
    {
        if (file->name == internalName)
        {
            if (file->data)
            {
                file->data = nullptr;
            }
            file->name = Name;
            file->offset = 0;
            return;
        }
    }
    Add(Name);
}
bool LibFiles::WriteData(FILE* stream, ObjFile* file, const ObjString& name)
{
    ObjIeeeIndexManager im1;
    ObjFactory fact1(&im1);
    ObjIeee ieee(name.c_str());
    return ieee.Write(stream, file, &fact1);
}
bool LibFiles::WriteNames(FILE* stream)
{
    for (auto&& file : *this)
    {
        const char* p = file->name.c_str();
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
        if (fwrite(q, strlen(q) + 1, 1, stream) != 1)
            return false;
    }
    return true;
}
bool LibFiles::WriteOffsets(FILE* stream)
{
    for (auto&& file : *this)
    {
        ObjInt ofs = file->offset;
        if (fwrite(&ofs, 4, 1, stream) != 1)
            return false;
    }
    return true;
}
bool LibFiles::Align(FILE* stream, int align)
{
    char buf[LibManager::ALIGN];
    memset(buf, 0, align);
    int n = ftell(stream);
    if (n % align)
        if (fwrite(buf, align - n % align, 1, stream) != 1)
            return false;
    return true;
}
bool LibFiles::ReadFiles(FILE* stream, ObjFactory* factory)
{
    bool rv = true;
    bool done = false;
    while (!done)
    {
        done = true;
        for (auto it = begin(); it != end();)
        {
            auto itn = it;
            ++it;
            if (!(*itn)->data)
            {
                if ((*itn)->offset)
                {
                    if (fseek(stream, (*itn)->offset, SEEK_SET))
                        return false;
                    (*itn)->data = ReadData(stream, (*itn)->name, factory);
                    if (!(*itn)->data)
                    {
                        std::cout << "Error: Syntax error in module '" << (*itn)->name << "'" << std::endl;
                        files.erase(itn);
                        done = false;
                        rv = false;
                        break;
                    }
                    // leaving export records alone if they were added previosly without --noexport
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
                        else if (noExport)
                        {
                            (*itn)->data->ExportClear();
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
bool LibFiles::WriteFiles(FILE* stream, ObjInt align)
{
    int i = 0;
    for (auto&& file : *this)
    {
        if (!Align(stream, align))
            return false;
        file->offset = ftell(stream);
        if (!WriteData(stream, file->data, file->name))
            return false;
    }
    return true;
}
