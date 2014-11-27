/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
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
    for (ResourceContainer::NumberedTypes::iterator it = resources.numberedTypes.begin(); it != resources.numberedTypes.end(); ++ it)
    {
        dirCount++;
        entryCount++;
        for (ResourceContainer::NumberedIds::iterator nt = it->second.numberedIds.begin(); nt != it->second.numberedIds.end(); ++ nt)
        {
            dirCount++;
            entryCount+= 2;
            if (nt->second.data)
            {
                dataCount++;
                dataSize += (nt->second.length + 3) & ~3;
            }
        }
        for (ResourceContainer::NamedIds::iterator nt = it->second.namedIds.begin(); nt != it->second.namedIds.end(); ++ nt)
        {
            dirCount++;
            entryCount+= 2;
            nameSize += nt->first.size() + 2;
            if (nt->second.data)
            {
                dataCount++;
                dataSize += (nt->second.length + 3) & ~3;
            }
        }
    }
    for (ResourceContainer::NamedTypes::iterator it = resources.namedTypes.begin(); it != resources.namedTypes.end(); ++ it)
    {
        dirCount++;
        entryCount++;
        nameSize += it->first.size() + 2;
        for (ResourceContainer::NumberedIds::iterator nt = it->second.numberedIds.begin(); nt != it->second.numberedIds.end(); ++ nt)
        {
            dirCount++;
            entryCount+= 2;
            if (nt->second.data)
            {
                dataCount++;
                dataSize += (nt->second.length + 3) & ~3;
            }
        }
        for (ResourceContainer::NamedIds::iterator nt = it->second.namedIds.begin(); nt != it->second.namedIds.end(); ++ nt)
        {
            dirCount++;
            entryCount+= 2;
            nameSize += nt->first.size() + 2;
            if (nt->second.data)
            {
                dataCount++;
                dataSize += (nt->second.length + 3) & ~3;
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
    for (ResourceContainer::NamedTypes::iterator it = resources.namedTypes.begin(); it != resources.namedTypes.end(); ++ it)
    {
        types[i].subdir_or_data = SUBDIR |( ((unsigned char *)dir) - data);
        types[i++].rva_or_id = RVA | ((unsigned char *)name - data);
        const WCHAR *p = it->first.c_str();
        *name++ = it->first.size();
        while(*p)
        {
            *name++ = *p++;
        }
        *name++ = *p++;
        dir->time = time(0);
        dir->name_entry = it->second.namedIds.size() ;
        dir->ident_entry = it->second.numberedIds.size();
        Entry *ids = (Entry *)(dir + 1);
        dir = (Dir *)(ids + dir->name_entry + dir->ident_entry);
        int j = 0;
        for (ResourceContainer::NamedIds::iterator nt = it->second.namedIds.begin(); nt != it->second.namedIds.end(); ++ nt)
        {
            ids[j].subdir_or_data = SUBDIR | ((unsigned char *)dir - data);
            ids[j++].rva_or_id = RVA | ((unsigned char *)name - data);
            const WCHAR *p = nt->first.c_str();
            *name++ = nt->first.size();
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
            dataEntries->size = nt->second.length;
            dataEntries->codepage = nt->second.language;
            dataEntries++;
            memcpy(dataPos, nt->second.data, nt->second.length);
            dataPos += (nt->second.length + 3) & ~3;
        }
        for (ResourceContainer::NumberedIds::iterator nt = it->second.numberedIds.begin(); nt != it->second.numberedIds.end(); ++ nt)
        {
            ids[j].subdir_or_data = SUBDIR |((unsigned char *)dir - data);
            ids[j++].rva_or_id = nt->first;
            dir->time = time(0);
            dir->name_entry = 0;
            dir->ident_entry = 1;
            Entry *subtypes = (Entry *)(dir + 1);
            dir = (Dir *)(subtypes + 1);
            subtypes->subdir_or_data = (unsigned char *)dataEntries - data;
            subtypes->rva_or_id = 0;
            dataEntries->rva = (unsigned)(dataPos - data) + virtual_addr;
            dataEntries->size = nt->second.length;
            dataEntries->codepage = nt->second.language;
            dataEntries++;
            memcpy(dataPos, nt->second.data, nt->second.length);
            dataPos += (nt->second.length + 3) & ~3;
        }
    }
    for (ResourceContainer::NumberedTypes::iterator it = resources.numberedTypes.begin(); it != resources.numberedTypes.end(); ++ it)
    {
        types[i].subdir_or_data = SUBDIR |( ((unsigned char *)dir) - data);
        types[i++].rva_or_id = it->first;
        dir->time = time(0);
        dir->name_entry = it->second.namedIds.size() ;
        dir->ident_entry = it->second.numberedIds.size();
        Entry *ids = (Entry *)(dir + 1);
        dir = (Dir *)(ids + dir->name_entry + dir->ident_entry);
        int j = 0;
        for (ResourceContainer::NamedIds::iterator nt = it->second.namedIds.begin(); nt != it->second.namedIds.end(); ++ nt)
        {
            ids[j].subdir_or_data = SUBDIR |((unsigned char *)dir - data);
            ids[j++].rva_or_id = RVA | ((unsigned char *)name - data);
            const WCHAR *p = nt->first.c_str();
            *name++ = nt->first.size();
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
            dataEntries->size = nt->second.length;
            dataEntries->codepage = nt->second.language;
            dataEntries++;
            memcpy(dataPos, nt->second.data, nt->second.length);
            dataPos += (nt->second.length + 3) & ~3;
        }
        for (ResourceContainer::NumberedIds::iterator nt = it->second.numberedIds.begin(); nt != it->second.numberedIds.end(); ++ nt)
        {
            ids[j].subdir_or_data = SUBDIR |((unsigned char *)dir - data);
            ids[j++].rva_or_id = nt->first;
            dir->time = time(0);
            dir->name_entry = 0;
            dir->ident_entry = 1;
            Entry *subtypes = (Entry *)(dir + 1);
            dir = (Dir *)(subtypes + 1);
            subtypes->subdir_or_data = (unsigned char *)dataEntries - data;
            subtypes->rva_or_id = 0;
            dataEntries->rva = (unsigned)(dataPos - data) + virtual_addr;
            dataEntries->size = nt->second.length;
            dataEntries->codepage = nt->second.language;
            dataEntries++;
            memcpy(dataPos, nt->second.data, nt->second.length);
            dataPos += (nt->second.length + 3) & ~3;
        }

    }
    
    endVa = ObjectAlign(objectAlign, endVa + size);
    endPhys = ObjectAlign(fileAlign, endPhys + initSize);	
}
