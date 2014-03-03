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
#include "ObjFile.h"
#include "ObjSymbol.h"
#include "ObjExpression.h"

#include <string>
#include <ctype.h>
#include <map>
#include <iostream>
class namelt
{
    public:
        bool operator () (ObjExportSymbol *left, ObjExportSymbol *right)
        {
            return left->GetName() < right->GetName();
        }
} ;
void PEExportObject::Setup(ObjInt &endVa, ObjInt &endPhys)
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
    std::map<std::string, ObjInt> publics;
    for (ObjFile::SymbolIterator it = file->PublicBegin(); it != file->PublicEnd(); ++it)
    {
        publics[(*it)->GetName()] = (*it)->GetOffset()->Eval(0);
    }


    int pos = moduleName.find_last_of("\\");
    if (pos == std::string::npos)
    {
        pos = moduleName.find_last_of(":");
        if (pos == std::string::npos)
            pos = 0;
    }
    std::string name = moduleName.substr(pos);

    size = initSize = sizeof(Header) + 1 + name.size();

    unsigned minOrd=0xffffffff; /* max ordinal num */
    unsigned maxOrd=0;
    unsigned count = 0;
    std::set<ObjExportSymbol *, namelt> names;
    for (ObjFile::SymbolIterator it = file->ExportBegin(); it != file->ExportEnd(); ++it)
    {
        ObjExportSymbol *s = (ObjExportSymbol *)(*it);
        names.insert(s);
        if (!s->GetByOrdinal())
        {
            initSize = (size += s->GetName().size() + 1 + 6);
        }
        if (s->GetOrdinal() != 0xffffffff)
        {
            int n = s->GetOrdinal();
            if (n < minOrd)
                minOrd = n;
            if (n > maxOrd)
                maxOrd = n;
        }
        count++;
    }				
    if (maxOrd >= minOrd)
    {
        count = count > (maxOrd-minOrd + 1) ? count : maxOrd-minOrd + 1;
    }
    else
    {
        minOrd = 1;
        maxOrd = count + minOrd - 1;
    }
    initSize = (size +=	4 * count);
    data = new unsigned char[initSize];
    memset(data, 0, initSize);
    Header *header = (Header *)data;

    header->time = time(0);
    header->ord_base = minOrd;
    header->n_eat_entries = count;
    header->n_name_ptrs = names.size();
    unsigned *rvaTable = (unsigned *)(data + sizeof(Header));
    unsigned *nameTable = (unsigned *)(((unsigned char *)rvaTable) + 4 * count);
    unsigned short *ordinalTable = (unsigned short *)(((unsigned char *)nameTable) + 4 * names.size());
    unsigned char *stringTable = (unsigned char *)(((unsigned char *)ordinalTable) + 2 * names.size());

    header->address_rva = virtual_addr + ((unsigned char *)rvaTable) - data;
    header->name_rva = virtual_addr + ((unsigned char *)nameTable) - data;
    header->ordinal_rva = virtual_addr + ((unsigned char *)ordinalTable) - data;

    /* process numbered exports */
    for (ObjFile::SymbolIterator it = file->ExportBegin(); it != file->ExportEnd(); ++it)
    {
        ObjExportSymbol *s = (ObjExportSymbol *)(*it);
        if (s->GetByOrdinal())
        {
            int n = s->GetOrdinal();
            if (rvaTable[n - minOrd] != 0)
                std::cout << "Warning: Export '" << s->GetName() << "' duplicates an ordinal" << std::endl;
            int addr = publics[s->GetName()];
            if (addr == 0)
                std::cout << "Warning: Export '" << s->GetName() << "' has no related public" << std::endl;
                
            rvaTable[n - minOrd] = addr - imageBase;
        }
    }			
                
    /* process non-numbered exports */
    pos = 0;
    for (ObjFile::SymbolIterator it = file->ExportBegin(); it != file->ExportEnd(); ++it)
    {
        ObjExportSymbol *s = (ObjExportSymbol *)(*it);
        if (!s->GetByOrdinal())
        {
            while(rvaTable[pos])
                pos++;
            s->SetOrdinal(pos + minOrd);
            int addr = publics[s->GetName()];
            if (addr == 0)
                std::cout << "Warning: Export '" << s->GetName() << "' has no related public" << std::endl;
            rvaTable[pos] = addr - imageBase;
        }
    }
    // process named exports
    for (std::set<ObjExportSymbol *, namelt>::iterator it = names.begin(); it != names.end(); ++it)
    {
        *nameTable++ = (unsigned)((unsigned char *)stringTable - data + virtual_addr);
        *ordinalTable++ = (*it)->GetOrdinal() - minOrd;
        strcpy((char *)stringTable, (*it)->GetName().c_str());
        stringTable += (*it)->GetName().size();
        *stringTable++ = 0;
    }
    // throw in the DLL name
    if (name.size())
    {
        header->exe_name_rva = (unsigned)((unsigned char *)stringTable - data + virtual_addr);
        for (int i=0; i < name.size(); i++)
            *stringTable++ = toupper(name[i]);
        *stringTable++ = 0;
    }
    endVa = ObjectAlign(objectAlign, endVa + size);
    endPhys = ObjectAlign(fileAlign, endPhys + initSize);
}
