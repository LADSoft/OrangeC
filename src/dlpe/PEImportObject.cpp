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
#include "Utils.h"
#include "ObjSymbol.h"
#include "Objfile.h"
#include "ObjExpression.h"

#include <ctype.h>
#include <map>
#include <string.h>

void PEImportObject::Setup(ObjInt &endVa, ObjInt &endPhys)
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
    raw_addr = endPhys;
    std::map<ObjString, ObjSymbol *> externs;
    for (ObjFile::SymbolIterator it = file->ExternalBegin(); it != file->ExternalEnd(); ++it)
    {
        externs[(*it)->GetName()] = (*it);
    }
    std::map<std::string, Module *> modules;
    int nameSize = 0;
    int impNameSize = 0;
    int importCount = 0;
    int dllCount = 0;
        std::string name; // moved out of loop because of annoying OPENWATCOM bug
    for (ObjFile::SymbolIterator it = file->ImportBegin(); it != file->ImportEnd(); ++it)
    {
        ObjImportSymbol *s = (ObjImportSymbol *)(*it);
        // uppercase the module name for NT... 98 doesn't need it but can accept it
        name = s->GetDllName();
        for (int i =0; i < name.size(); i++)
            name[i] = toupper(name[i]);
        s->SetDllName(name);
        if (externs.find((*it)->GetName()) != externs.end())
        {
            Module *m = modules[s->GetDllName()];
            int sz = s->GetDllName().size() + 1;
            if (sz & 1)
                sz ++;
            nameSize += sz;
            if (m == NULL)
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
                sz ++;
            impNameSize += sz;
            m->publicNames.push_back(s->GetName());
            m->ordinals.push_back(s->GetByOrdinal() ? s->GetOrdinal() : 0xffffffff);
            importCount ++;
        }
    }
    data = new unsigned char[(modules.size() + 1) * sizeof(Dir) +
                             ((nameSize + 3) & ~3) + (importCount + dllCount) * sizeof(Entry) * 2 +
                             ((impNameSize + 3) & ~3)];
    Dir *dirPos = (Dir *)data;
    char *namePos = (char *)data + sizeof(Dir) * (modules.size() + 1);
    Entry *lookupPos = (Entry *)((char *)namePos + ((nameSize + 3) & ~3));
    char *hintPos = ((char *)lookupPos)	+ (importCount + dllCount) * sizeof(Entry);
    Entry *addressPos = (Entry *)(hintPos + ((impNameSize + 3) & ~3));
    size = initSize = (unsigned)(((unsigned char *)addressPos) - data + (importCount + dllCount) * sizeof(Entry));
    memset(data, 0, size); // note this does clean out some areas we deliberately are not initializing
    for (std::map<std::string, Module *>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        dirPos->time = 0;
        dirPos->version = 0;
        dirPos->dllName = (unsigned char *)namePos - data + virtual_addr;
        dirPos->thunkPos = ((unsigned char *)lookupPos) - data + virtual_addr;
        dirPos->thunkPos2 = ((unsigned char *)addressPos) - data + virtual_addr;
        dirPos++;
        strcpy(namePos, it->first.c_str());
        int n = it->first.size() + 1;
        if (n & 1)
            n++;
        namePos += n;
        for (int i = 0; i < it->second->externalNames.size(); i++)
        {
            const std::string &str = it->second->externalNames[i];
            if (str.size())
            {
                lookupPos->ord_or_rva = (unsigned char *)hintPos - data + virtual_addr;
                addressPos->ord_or_rva = (unsigned char *) hintPos - data + virtual_addr;
                *(short *)hintPos = 0; //it->second->ordinals[i];
                hintPos += 2;
                strcpy(hintPos, str.c_str());
                int n = str.size() + 1;
                if (n & 1)
                    n++;
                hintPos += n;
            }
            else
            {
                lookupPos->ord_or_rva = addressPos->ord_or_rva = it->second->ordinals[i] | IMPORT_BY_ORDINAL;
            }
            // next up we make a thunk that will get us from the rel calls genned by
            // the compiler to the import table;		
            ObjSymbol *sym = externs[it->second->publicNames[i]];
            int en = sym->GetIndex();
            for (std::deque<PEObject *>::iterator it1 = objects.begin(); it1 != objects.end(); ++ it1)
            {
                ObjInt val;
                if ((val = (*it1)->SetThunk(en, ((unsigned char *)lookupPos) - data + virtual_addr + imageBase)) != -1)
                    sym->SetOffset(new ObjExpression(val));
            }
            lookupPos++;
            addressPos++;
        }
        // skip the null entry at the end of a module
        lookupPos++;
        addressPos++;
    }
    for (std::map<std::string, Module *>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        Module *p = it->second;
        delete p;
    }

    endVa = ObjectAlign(objectAlign, endVa + size);
    endPhys = ObjectAlign(fileAlign, endPhys + initSize);
}
