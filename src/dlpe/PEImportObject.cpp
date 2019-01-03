/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
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

#include "PEObject.h"
#include "Utils.h"
#include "ObjSymbol.h"
#include "ObjFile.h"
#include "ObjExpression.h"

#include <cctype>
#include <map>
#include <cstring>

void PEImportObject::Setup(ObjInt& endVa, ObjInt& endPhys)
{
    if (virtual_addr == 0)
    {
        virtual_addr = endVa;
    }
    else
    {
        if (virtual_addr != endVa)
            Utils::fatal("Internal error");
    }
    size = 0;
    raw_addr = endPhys;
    std::map<ObjString, ObjSymbol*> externs;
    for (auto it = file->ExternalBegin(); it != file->ExternalEnd(); ++it)
    {
        externs[(*it)->GetName()] = (*it);
    }
    std::map<std::string, Module*> modules;
    int nameSize = 0;
    int impNameSize = 0;
    int importCount = 0;
    int dllCount = 0;
    std::string name;  // moved out of loop because of annoying OPENWATCOM bug
    for (auto it = file->ImportBegin(); it != file->ImportEnd(); ++it)
    {
        ObjImportSymbol* s = (ObjImportSymbol*)(*it);
        // uppercase the module name for NT... 98 doesn't need it but can accept it
        name = s->GetDllName();
        for (int i = 0; i < name.size(); i++)
            name[i] = toupper(name[i]);
        s->SetDllName(name);
        if (externs.find((*it)->GetName()) != externs.end())
        {
            Module* m = modules[s->GetDllName()];
            int sz = s->GetDllName().size() + 1;
            if (sz & 1)
                sz++;
            nameSize += sz;
            if (m == nullptr)
            {
                modules[s->GetDllName()] = m = new Module;
                m->module = s->GetDllName();
                dllCount++;
            }
            if (s->GetExternalName().size() == 0)
            {
                m->externalNames.push_back(s->GetName());
                sz = s->GetName().size() + 1;
            }
            else
            {
                m->externalNames.push_back(s->GetExternalName());
                sz = s->GetExternalName().size() + 1;
            }
            sz += 2;
            if (sz & 1)
                sz++;
            impNameSize += sz;
            m->publicNames.push_back(s->GetName());
            m->ordinals.push_back(s->GetByOrdinal() ? s->GetOrdinal() : 0xffffffff);
            importCount++;
        }
    }
    data = new unsigned char[(modules.size() + 1) * sizeof(Dir) + ((nameSize + 3) & ~3) +
                             (importCount + dllCount) * sizeof(Entry) * 2 + ((impNameSize + 3) & ~3)];
    Dir* dirPos = (Dir*)data;
    char* namePos = (char*)data + sizeof(Dir) * (modules.size() + 1);
    Entry* lookupPos = (Entry*)((char*)namePos + ((nameSize + 3) & ~3));
    char* hintPos = ((char*)lookupPos) + (importCount + dllCount) * sizeof(Entry);
    Entry* addressPos = (Entry*)(hintPos + ((impNameSize + 3) & ~3));
    size = initSize = (unsigned)(((unsigned char*)addressPos) - data + (importCount + dllCount) * sizeof(Entry));
    memset(data, 0, size);  // note this does clean out some areas we deliberately are not initializing

    for (auto module : modules)
    {
        dirPos->time = 0;
        dirPos->version = 0;
        dirPos->dllName = (unsigned char*)namePos - data + virtual_addr;
        dirPos->thunkPos = ((unsigned char*)lookupPos) - data + virtual_addr;
        dirPos->thunkPos2 = ((unsigned char*)addressPos) - data + virtual_addr;
        dirPos++;
        strcpy(namePos, module.first.c_str());
        int n = module.first.size() + 1;
        if (n & 1)
            n++;
        namePos += n;
        for (int i = 0; i < module.second->externalNames.size(); i++)
        {
            const std::string& str = module.second->externalNames[i];
            if (!str.empty())
            {
                lookupPos->ord_or_rva = (unsigned char*)hintPos - data + virtual_addr;
                addressPos->ord_or_rva = (unsigned char*)hintPos - data + virtual_addr;
                *(short*)hintPos = 0;
                hintPos += 2;
                strcpy(hintPos, str.c_str());
                int n = str.size() + 1;
                if (n & 1)
                    n++;
                hintPos += n;
            }
            else
            {
                lookupPos->ord_or_rva = addressPos->ord_or_rva = module.second->ordinals[i] | IMPORT_BY_ORDINAL;
            }
            ObjSymbol* sym = externs[module.second->publicNames[i]];
            sym->SetOffset(new ObjExpression(((unsigned char*)lookupPos) - data + virtual_addr + imageBase));

            lookupPos++;
            addressPos++;
        }
        // skip the null entry at the end of a module
        lookupPos++;
        addressPos++;
    }
    for (auto module : modules)
    {
        Module* p = module.second;
        delete p;
    }

    endVa = ObjectAlign(objectAlign, endVa + size);
    endPhys = ObjectAlign(fileAlign, endPhys + initSize);
}
