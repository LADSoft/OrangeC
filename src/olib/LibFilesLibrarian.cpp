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
#include "LibManager.h"
#include "LibFiles.h"
#include "ObjFile.h"
#include "ObjIeee.h"
#include "ObjFactory.h"
#include "CmdFiles.h"
#include <iostream>
#include <string.h>

void LibFiles::Add(ObjFile &obj)
{
    for (int i=0; i < files.size(); i++)
        if (files[i]->name == obj.GetName())
        {
            std::cout << "Warning: module '" << files[i]->name.c_str() << "' already exists in library, it won't be added" << std:: endl;
            return;
        }
    FileDescriptor *newFile = new FileDescriptor(obj.GetName());
    newFile->data = &obj;
    files.push_back(newFile);
}
void LibFiles::Add(const ObjString &Name)
{
    unsigned npos = Name.find_last_of(CmdFiles::DIR_SEP);
    std::string internalName = Name;
    if (npos != std::string::npos)
        internalName = Name.substr(npos+1);
    for (int i=0; i < files.size(); i++)
        if (files[i]->name == internalName)
        {
            std::cout << "Warning: module '" << Name.c_str() << "' already exists in library, it won't be added" << std:: endl;
            return;
        }
    FileDescriptor *newFile = new FileDescriptor(Name);
    files.push_back(newFile);
}
void LibFiles::Remove(const ObjString &Name)
{
    unsigned npos = Name.find_last_of(CmdFiles::DIR_SEP);
    std::string internalName = Name;
    if (npos != std::string::npos)
        internalName = Name.substr(npos+1);
    for (FileIterator it = FileBegin(); it != FileEnd(); ++it)
    {
        if ((*it)->name == internalName)
        {
            FileDescriptor *t = *it;
            delete t;
            files.erase(it);
            return;
        }
    }
    std::cout << "Warning: Module '" << Name.c_str() << "' not in library and could not be removed" << std::endl;;
}
void LibFiles::Extract(FILE *stream, const ObjString &Name)
{
    unsigned npos = Name.find_last_of(CmdFiles::DIR_SEP);
    std::string internalName = Name;
    if (npos != std::string::npos)
        internalName = Name.substr(npos+1);
    int count = 0;
    ObjIeeeIndexManager im1;
    ObjFactory fact1(&im1);
    for (FileIterator it = FileBegin(); it != FileEnd(); ++it)
    {
        if ((*it)->name == internalName)
        {
            ObjFile *p = LoadModule(stream, count, &fact1);
            if (p)
            {
                FILE *ostr = fopen(Name.c_str(), "wb");
                if (ostr != NULL)
                {
                    WriteData(ostr, p, (*it)->name);
                    fclose(ostr);
                }
                else
                {
                    std::cout << "Warning: Module '" << Name.c_str() << "' not extracted, could not open output file" << std::endl;
                }
            }
            else
            {
                std::cout << "Warning: Module '" << Name.c_str() << "' not extracted, library corrupt" << std::endl;
            }
            return;
        }
    }
    std::cout << "Warning: Module '" << Name.c_str() << "' not in library and could not be extracted" << std::endl;
}
void LibFiles::Replace(const ObjString &Name)
{
    unsigned npos = Name.find_last_of(CmdFiles::DIR_SEP);
    std::string internalName = Name;
    if (npos != std::string::npos)
        internalName = Name.substr(npos+1);
    for (FileIterator it = FileBegin(); it != FileEnd(); ++it)
    {
        if ((*it)->name == internalName)
        {
            if ((*it)->data)
            {
                delete (*it)->data;
                (*it)->data = NULL;
            }
            (*it)->name = Name;
            (*it)->offset = 0;
            return;
        }
    }
    std::cout << "Warning: Module '" << Name.c_str() << "' not in library and will be added" << std::endl;
    Add(Name);        
}
void LibFiles::WriteData(FILE *stream, ObjFile *file, const ObjString &name)
{
    ObjIeeeIndexManager im1;
    ObjFactory fact1(&im1);
    ObjIeee ieee(name.c_str());
    ieee.Write(stream, file, &fact1);
}
void LibFiles::WriteNames(FILE *stream)
{
    for (FileIterator it = FileBegin(); it != FileEnd(); ++it)
    {
        const char *p = (*it)->name.c_str();
        const char *q = strrchr(p, '\\');
        if (q)
            q++;
        else
            q = p;
        fwrite(q, strlen(q) + 1, 1, stream);
    }
}
void LibFiles::WriteOffsets(FILE *stream)
{
    for (FileIterator it = FileBegin(); it != FileEnd(); ++it)
    {
        ObjInt ofs = (*it)->offset;
        fwrite(&ofs, 4, 1, stream);
    }
}
void LibFiles::Align(FILE *stream, int align)
{
    char buf[LibManager::ALIGN];
    memset(buf, 0, align);
    int n = ftell(stream);
    if ( n % align)
        fwrite(buf, align - n %align, 1, stream);
}
bool LibFiles::ReadFiles(FILE *stream, ObjFactory *factory)
{
    bool rv = true;
    bool done = false;
    while (!done)
    {
        done = true;
        for (FileIterator it = FileBegin(); it != FileEnd();)
        {
            FileIterator itn = it;
            ++it;
            if (!(*itn)->data)
            {
                if ((*itn)->offset)
                {
                    fseek(stream, (*itn)->offset, SEEK_SET);
                    (*itn)->data = ReadData(stream, (*itn)->name, factory);
                    if (!(*itn)->data)
                    {
                        std::cout << "Error: Syntax error in module '" << (*itn)->name.c_str() << "'" << std::endl;
                        FileDescriptor *t = *itn;
                        delete t;
                        files.erase(itn);
                        done = false;
                        rv = false;
                        break;
                    }
                }
                else
                {
                    FILE *istr = fopen((*itn)->name.c_str(), "rb");
                    if (istr != NULL)
                    {
                        (*itn)->data = ReadData(istr, (*itn)->name, factory);
                        fclose(istr);
                        if (!(*itn)->data)
                        {
                            std::cout << "Error: Syntax error in module '" << (*itn)->name.c_str() << "'" << std::endl;
                            FileDescriptor *t = *itn;
                            delete t;
                            files.erase(itn);
                            done = false;
                            rv = false;
                            break;
                        }
                    }
                    else
                    {
                        std::cout << "Error: Module '" << (*itn)->name.c_str() << "' does not exist" << std::endl;
                        FileDescriptor *t = *itn;
                        delete t;
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
void LibFiles::WriteFiles(FILE *stream, ObjInt align)
{
    int i =0;
    for (FileIterator it = FileBegin(); it != FileEnd(); ++it)
    {
        Align(stream, align);
        (*it)->offset = ftell(stream);
        WriteData(stream, (*it)->data, (*it)->name);
    }
}
