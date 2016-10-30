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
#include "PEObject.h"
#include "ResourceContainer.h"
#include "Utils.h"
#include "windows.h"
#include <time.h>
#include <string.h>

void PEResourceObject::Setup(ObjInt &endVa, ObjInt &endPhys)
{
    if (resources.size() == 0)
        return;
    if (!resources.LoadFiles())
        Utils::fatal("Error loading resources");
    if (virtual_addr == 0)
    {
        virtual_addr = endVa;
    }
    else
    {
        if (virtual_addr != endVa)
            Utils::fatal("Internal error");
    }
    raw_addr = endPhys;
    
    // count
    int dirCount = 1;
    int entryCount = 0;
    int dataCount = 0;
    unsigned dataSize = 0;
    unsigned nameSize = 0; // size is in characters not bytes
    for (auto type : resources.numberedTypes)
    {
        dirCount++;
        entryCount++;
        for (auto number : type.second.numberedIds)
        {
            dirCount++;
            entryCount+= 2;
            if (number.second.data)
            {
                dataCount++;
                dataSize += (number.second.length + 3) & ~3;
            }
        }
        for (auto name : type.second.namedIds)
        {
            dirCount++;
            entryCount+= 2;
            nameSize += name.first.size() + 2;
            if (name.second.data)
            {
                dataCount++;
                dataSize += (name.second.length + 3) & ~3;
            }
        }
    }
    for (auto type : resources.namedTypes)
    {
        dirCount++;
        entryCount++;
        nameSize += type.first.size() + 2;
        for (auto number : type.second.numberedIds)
        {
            dirCount++;
            entryCount+= 2;
            if (number.second.data)
            {
                dataCount++;
                dataSize += (number.second.length + 3) & ~3;
            }
        }
        for (auto name : type.second.namedIds)
        {
            dirCount++;
            entryCount+= 2;
            nameSize += name.first.size() + 2;
            if (name.second.data)
            {
                dataCount++;
                dataSize += (name.second.length + 3) & ~3;
            }
        }
    }
    
    nameSize = (nameSize + 1) & -2;
    initSize = size = dirCount * sizeof(Dir) + entryCount * sizeof(Entry) +
                        dataCount * sizeof(DataEntry) + nameSize * 2 + dataSize;
    data =  new unsigned char [initSize];
    memset(data, 0, initSize);
    Dir *dir = (Dir *)data;
    unsigned short *name = (unsigned short *)(((char *)data) + dirCount * sizeof(Dir) + entryCount * sizeof(Entry));
    DataEntry * dataEntries  = (DataEntry *)(((char *)name) + nameSize * 2);
    unsigned char *dataPos = (unsigned char *)(dataEntries + dataCount);
    dir->time = time(0);
    dir->name_entry = resources.namedTypes.size() ;
    dir->ident_entry = resources.numberedTypes.size();
    Entry *types = (Entry *)(dir + 1);
    dir = (Dir *)(types + dir->name_entry + dir->ident_entry);
    int i = 0;
    for (auto type : resources.namedTypes)
    {
        types[i].subdir_or_data = SUBDIR |( ((unsigned char *)dir) - data);
        types[i++].rva_or_id = RVA | ((unsigned char *)name - data);
        const WCHAR *p = type.first.c_str();
        *name++ = type.first.size();
        while(*p)
        {
            *name++ = *p++;
        }
        *name++ = *p++;
        dir->time = time(0);
        dir->name_entry = type.second.namedIds.size() ;
        dir->ident_entry = type.second.numberedIds.size();
        Entry *ids = (Entry *)(dir + 1);
        dir = (Dir *)(ids + dir->name_entry + dir->ident_entry);
        int j = 0;
        for (auto nameId : type.second.namedIds)
        {
            ids[j].subdir_or_data = SUBDIR | ((unsigned char *)dir - data);
            ids[j++].rva_or_id = RVA | ((unsigned char *)name - data);
            const WCHAR *p = nameId.first.c_str();
            *name++ = nameId.first.size();
            while(*p)
            {
                *name++ = *p++;
            }
            *name++ = *p++;
            dir->time = time(0);
            dir->name_entry = 0;
            dir->ident_entry = 1;
            Entry *subtypes = (Entry *)(dir + 1);
            dir = (Dir *)(subtypes + 1);
            subtypes->subdir_or_data = (unsigned char *)dataEntries - data;
            subtypes->rva_or_id = 0;
            dataEntries->rva = (unsigned)(dataPos - data) + virtual_addr;
            dataEntries->size = nameId.second.length;
            dataEntries->codepage = nameId.second.language;
            dataEntries++;
            memcpy(dataPos, nameId.second.data, nameId.second.length);
            dataPos += (nameId.second.length + 3) & ~3;
        }
        for (auto number : type.second.numberedIds)
        {
            ids[j].subdir_or_data = SUBDIR |((unsigned char *)dir - data);
            ids[j++].rva_or_id = number.first;
            dir->time = time(0);
            dir->name_entry = 0;
            dir->ident_entry = 1;
            Entry *subtypes = (Entry *)(dir + 1);
            dir = (Dir *)(subtypes + 1);
            subtypes->subdir_or_data = (unsigned char *)dataEntries - data;
            subtypes->rva_or_id = 0;
            dataEntries->rva = (unsigned)(dataPos - data) + virtual_addr;
            dataEntries->size = number.second.length;
            dataEntries->codepage = number.second.language;
            dataEntries++;
            memcpy(dataPos, number.second.data, number.second.length);
            dataPos += (number.second.length + 3) & ~3;
        }
    }
    for (auto type : resources.numberedTypes)
    {
        types[i].subdir_or_data = SUBDIR |( ((unsigned char *)dir) - data);
        types[i++].rva_or_id = type.first;
        dir->time = time(0);
        dir->name_entry = type.second.namedIds.size() ;
        dir->ident_entry = type.second.numberedIds.size();
        Entry *ids = (Entry *)(dir + 1);
        dir = (Dir *)(ids + dir->name_entry + dir->ident_entry);
        int j = 0;
        for (auto namedId : type.second.namedIds)
        {
            ids[j].subdir_or_data = SUBDIR |((unsigned char *)dir - data);
            ids[j++].rva_or_id = RVA | ((unsigned char *)name - data);
            const WCHAR *p = namedId.first.c_str();
            *name++ = namedId.first.size();
            while(*p)
            {
                *name++ = *p++;
            }
            *name++ = *p++;
            dir->time = time(0);
            dir->name_entry = 0;
            dir->ident_entry = 1;
            Entry *subtypes = (Entry *)(dir + 1);
            dir = (Dir *)(subtypes + 1);
            subtypes->subdir_or_data = (unsigned char *)dataEntries - data;
            subtypes->rva_or_id = 0;
            dataEntries->rva = (unsigned)(dataPos - data) + virtual_addr;
            dataEntries->size = namedId.second.length;
            dataEntries->codepage = namedId.second.language;
            dataEntries++;
            memcpy(dataPos, namedId.second.data, namedId.second.length);
            dataPos += (namedId.second.length + 3) & ~3;
        }
        for (auto number : type.second.numberedIds)
        {
            ids[j].subdir_or_data = SUBDIR |((unsigned char *)dir - data);
            ids[j++].rva_or_id = number.first;
            dir->time = time(0);
            dir->name_entry = 0;
            dir->ident_entry = 1;
            Entry *subtypes = (Entry *)(dir + 1);
            dir = (Dir *)(subtypes + 1);
            subtypes->subdir_or_data = (unsigned char *)dataEntries - data;
            subtypes->rva_or_id = 0;
            dataEntries->rva = (unsigned)(dataPos - data) + virtual_addr;
            dataEntries->size = number.second.length;
            dataEntries->codepage = number.second.language;
            dataEntries++;
            memcpy(dataPos, number.second.data, number.second.length);
            dataPos += (number.second.length + 3) & ~3;
        }

    }
    
    endVa = ObjectAlign(objectAlign, endVa + size);
    endPhys = ObjectAlign(fileAlign, endPhys + initSize);	
}
