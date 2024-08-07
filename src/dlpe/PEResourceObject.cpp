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

#include "dlPeMain.h"
#include "PEObject.h"
#include "ResourceContainer.h"
#include "Utils.h"
#include "ObjFactory.h"
#include <ctime>
#include <cstring>

void PEResourceObject::Setup(ObjInt& endVa, ObjInt& endPhys)
{
    if (resources.empty())
        return;
    if (!resources.LoadFiles())
        Utils::Fatal("Error loading resources");
    if (virtual_addr == 0)
    {
        virtual_addr = endVa;
    }
    else
    {
        if (virtual_addr != endVa)
            Utils::Fatal("Internal error");
    }
    raw_addr = endPhys;

    // count
    int dirCount = 1;
    int entryCount = 0;
    int dataCount = 0;
    unsigned dataSize = 0;
    unsigned nameSize = 0;  // size is in characters not bytes
    for (auto type : resources.numberedTypes)
    {
        dirCount++;
        entryCount++;
        for (auto number : type.second.numberedIds)
        {
            dirCount++;
            entryCount += 2;
            if (number.second.data)
            {
                dataCount++;
                dataSize += (number.second.length + 3) & ~3;
            }
        }
        for (auto name : type.second.namedIds)
        {
            dirCount++;
            entryCount += 2;
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
            entryCount += 2;
            if (number.second.data)
            {
                dataCount++;
                dataSize += (number.second.length + 3) & ~3;
            }
        }
        for (auto name : type.second.namedIds)
        {
            dirCount++;
            entryCount += 2;
            nameSize += name.first.size() + 2;
            if (name.second.data)
            {
                dataCount++;
                dataSize += (name.second.length + 3) & ~3;
            }
        }
    }

    nameSize = (nameSize + 1) & -2;
    initSize = size = dirCount * sizeof(Dir) + entryCount * sizeof(Entry) + dataCount * sizeof(DataEntry) + nameSize * 2 + dataSize;
    data = std::shared_ptr<unsigned char>(new unsigned char[initSize]);
    unsigned char* pdata = data.get();
    std::fill(pdata, pdata + initSize, 0);
    Dir* dir = (Dir*)pdata;
    unsigned short* name = (unsigned short*)(((char*)pdata) + dirCount * sizeof(Dir) + entryCount * sizeof(Entry));
    DataEntry* dataEntries = (DataEntry*)(((char*)name) + nameSize * 2);
    unsigned char* dataPos = (unsigned char*)(dataEntries + dataCount);
    dir->time = dlPeMain::TimeStamp();
    dir->name_entry = resources.namedTypes.size();
    dir->ident_entry = resources.numberedTypes.size();
    Entry* types = (Entry*)(dir + 1);
    dir = (Dir*)(types + dir->name_entry + dir->ident_entry);
    int i = 0;
    for (auto type : resources.namedTypes)
    {
        types[i].subdir_or_data = SUBDIR | (((unsigned char*)dir) - pdata);
        types[i++].rva_or_id = RVA | ((unsigned char*)name - pdata);
        const wchar_t* p = type.first.c_str();
        *name++ = type.first.size();
        while (*p)
        {
            *name++ = *p++;
        }
        *name++ = *p++;
        dir->time = dlPeMain::TimeStamp();
        dir->name_entry = type.second.namedIds.size();
        dir->ident_entry = type.second.numberedIds.size();
        Entry* ids = (Entry*)(dir + 1);
        dir = (Dir*)(ids + dir->name_entry + dir->ident_entry);
        int j = 0;
        for (auto nameId : type.second.namedIds)
        {
            ids[j].subdir_or_data = SUBDIR | ((unsigned char*)dir - pdata);
            ids[j++].rva_or_id = RVA | ((unsigned char*)name - pdata);
            const wchar_t* p = nameId.first.c_str();
            *name++ = nameId.first.size();
            while (*p)
            {
                *name++ = *p++;
            }
            *name++ = *p++;
            dir->time = dlPeMain::TimeStamp();
            dir->name_entry = 0;
            dir->ident_entry = 1;
            Entry* subtypes = (Entry*)(dir + 1);
            dir = (Dir*)(subtypes + 1);
            subtypes->subdir_or_data = (unsigned char*)dataEntries - pdata;
            subtypes->rva_or_id = 0;
            dataEntries->rva = RVA((unsigned)(dataPos - pdata));
            dataEntries->size = nameId.second.length;
            dataEntries->codepage = nameId.second.language;
            dataEntries++;
            memcpy(dataPos, nameId.second.data, nameId.second.length);
            dataPos += (nameId.second.length + 3) & ~3;
        }
        for (auto number : type.second.numberedIds)
        {
            ids[j].subdir_or_data = SUBDIR | ((unsigned char*)dir - pdata);
            ids[j++].rva_or_id = number.first;
            dir->time = dlPeMain::TimeStamp();
            dir->name_entry = 0;
            dir->ident_entry = 1;
            Entry* subtypes = (Entry*)(dir + 1);
            dir = (Dir*)(subtypes + 1);
            subtypes->subdir_or_data = (unsigned char*)dataEntries - pdata;
            subtypes->rva_or_id = 0;
            dataEntries->rva = RVA((unsigned)(dataPos - pdata));
            dataEntries->size = number.second.length;
            dataEntries->codepage = number.second.language;
            dataEntries++;
            memcpy(dataPos, number.second.data, number.second.length);
            dataPos += (number.second.length + 3) & ~3;
        }
    }
    for (auto type : resources.numberedTypes)
    {
        types[i].subdir_or_data = SUBDIR | (((unsigned char*)dir) - pdata);
        types[i++].rva_or_id = type.first;
        dir->time = dlPeMain::TimeStamp();
        dir->name_entry = type.second.namedIds.size();
        dir->ident_entry = type.second.numberedIds.size();
        Entry* ids = (Entry*)(dir + 1);
        dir = (Dir*)(ids + dir->name_entry + dir->ident_entry);
        int j = 0;
        for (auto namedId : type.second.namedIds)
        {
            ids[j].subdir_or_data = SUBDIR | ((unsigned char*)dir - pdata);
            ids[j++].rva_or_id = RVA | ((unsigned char*)name - pdata);
            const wchar_t* p = namedId.first.c_str();
            *name++ = namedId.first.size();
            while (*p)
            {
                *name++ = *p++;
            }
            *name++ = *p++;
            dir->time = dlPeMain::TimeStamp();
            dir->name_entry = 0;
            dir->ident_entry = 1;
            Entry* subtypes = (Entry*)(dir + 1);
            dir = (Dir*)(subtypes + 1);
            subtypes->subdir_or_data = (unsigned char*)dataEntries - pdata;
            subtypes->rva_or_id = 0;
            dataEntries->rva = RVA((unsigned)(dataPos - pdata));
            dataEntries->size = namedId.second.length;
            dataEntries->codepage = namedId.second.language;
            dataEntries++;
            memcpy(dataPos, namedId.second.data, namedId.second.length);
            dataPos += (namedId.second.length + 3) & ~3;
        }
        for (auto number : type.second.numberedIds)
        {
            ids[j].subdir_or_data = SUBDIR | ((unsigned char*)dir - pdata);
            ids[j++].rva_or_id = number.first;
            dir->time = dlPeMain::TimeStamp();
            dir->name_entry = 0;
            dir->ident_entry = 1;
            Entry* subtypes = (Entry*)(dir + 1);
            dir = (Dir*)(subtypes + 1);
            subtypes->subdir_or_data = (unsigned char*)dataEntries - pdata;
            subtypes->rva_or_id = 0;
            dataEntries->rva = RVA((unsigned)(dataPos - pdata));
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
