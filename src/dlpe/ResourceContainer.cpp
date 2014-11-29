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
#include "ResourceContainer.h"
#include "Utils.h"
#include <windows.h>
#include <fstream>
#include <iostream>

unsigned char ResourceContainer::resourceHeader[32] = 
{
   0,0,0,0,0x20,0,0,0,
   0xff,0xff,0,0,0xff,0xff,0,0,
   0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0
};
ResourceContainer::~ResourceContainer()
{
    for (NumberedTypes::iterator it = numberedTypes.begin(); it != numberedTypes.end(); ++ it)
    {
        for (NumberedIds::iterator nt = it->second.numberedIds.begin(); nt != it->second.numberedIds.end(); ++ nt)
        {
            if (nt->second.data)
                delete[] nt->second.data;
        }
        for (NamedIds::iterator nt = it->second.namedIds.begin(); nt != it->second.namedIds.end(); ++ nt)
        {
            if (nt->second.data)
                delete[] nt->second.data;
        }
    }
    for (NamedTypes::iterator it = namedTypes.begin(); it != namedTypes.end(); ++ it)
    {
        for (NumberedIds::iterator nt = it->second.numberedIds.begin(); nt != it->second.numberedIds.end(); ++ nt)
        {
            if (nt->second.data)
                delete[] nt->second.data;
        }
        for (NamedIds::iterator nt = it->second.namedIds.begin(); nt != it->second.namedIds.end(); ++ nt)
        {
            if (nt->second.data)
                delete[] nt->second.data;
        }
    }
}
bool ResourceContainer::LoadFiles()
{
    bool rv = true;
    for (std::deque<std::string>::iterator it = names.begin(); it != names.end(); ++it)
    {
        if (!LoadFile((*it)))
        {
            std::cout << "Error loading resource file '" << (*it).c_str() << "'" << std::endl;
            rv = false;
        }
    }
    return rv;
}
int ResourceContainer::GetId(unsigned short *hdrdata, int &i, int &id, std::wstring &name)
{
    if (hdrdata[i] == 0xffff)
    {
        i++;
        id = hdrdata[i++];
    }
    else
    {
        WCHAR buf[65535];
        int k = 0;
        while (hdrdata[i] != 0)
        {
            buf[k++] = hdrdata[i++];
        }
        buf[k++] = hdrdata[i++];
        name = buf;
    }
    return 0;
}
bool ResourceContainer::LoadFile(const std::string &name)
{
    std::fstream in(name.c_str(), std::ios::in | std::ios::binary);
    if (in == NULL)
        return false;
    unsigned char buf[sizeof(resourceHeader)];
    in.read((char *)buf, sizeof(resourceHeader));
    if (memcmp(buf, resourceHeader, sizeof(resourceHeader)))
        return false;
    while (1)
    {
        unsigned hdr[2];
        in.read((char *)hdr, sizeof(hdr));
        if (in.eof())
            break;
        unsigned short *hdrdata= new unsigned short[hdr[1]/2];
        in.read((char *)hdrdata, hdr[1] - sizeof(hdr));
        if (in.fail())
        {
            delete[] hdrdata;
            return false;
        }
        int typeId = 0;
        std::wstring type;
        int i = 0;
        GetId(hdrdata, i, typeId, type);
        int nameId = 0;
        std::wstring name;
        GetId(hdrdata, i, nameId, name);
        if (i & 1)
            i ++;
        int lang = hdrdata[i+3];
        i += 8; // past postheader
        delete[] hdrdata;
        // past end of header?
        if (i*2+8 > hdr[1])
            return false;
        unsigned char *data = new unsigned char[hdr[0]];
        in.read((char *)data, hdr[0]);
        if (in.fail())
        {
            delete[] data;
            return false;
        }
        int n = in.tellg();
        // align after data
        if (n & 3)
        {
            n += 4 - (n & 3);
            in.seekg(n);
        }
        InsertResource(typeId, type, nameId, name, data, hdr[0], lang);
    }
    return true;
}
void ResourceContainer::InsertResource(int typeId, std::wstring type, int nameId, std::wstring name, unsigned char *data, size_t len, int language)
{
    ResourceData d, n;
    n.sig = SIGNAT;
    n.data = data;
    n.length = len;
    n.language = language;
    if (type.size() == 0)
    {
        if (name.size() == 0)
        {
            d = numberedTypes[typeId].numberedIds[nameId];
            numberedTypes[typeId].numberedIds[nameId] = n;
        }
        else
        {
            d = numberedTypes[typeId].namedIds[name];
            numberedTypes[typeId].namedIds[name] = n;
        }
    }
    else
    {
        if (name.size() == 0)
        {
            d = namedTypes[type].numberedIds[nameId];
            namedTypes[type].numberedIds[nameId] = n;
        }
        else
        {
            d = namedTypes[type].namedIds[name];
            namedTypes[type].namedIds[name] = n;
        }
    }
    if (d.sig == SIGNAT)
    {
        Utils::fatal("Cannot resolve multiple resources with same ID");
    }
}
