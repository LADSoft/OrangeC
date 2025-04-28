/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

#include "ResourceContainer.h"
#include "Utils.h"
#include <fstream>
#include <iostream>
#include <cstring>

unsigned char ResourceContainer::resourceHeader[32] = {0, 0, 0, 0, 0x20, 0, 0, 0, 0xff, 0xff, 0, 0, 0xff, 0xff, 0, 0,
                                                       0, 0, 0, 0, 0,    0, 0, 0, 0,    0,    0, 0, 0,    0,    0, 0};
ResourceContainer::~ResourceContainer()
{
    try
    {
        for (auto type : numberedTypes)
        {
            for (const auto& nt : type.second.numberedIds)
                delete[] nt.second.data;
            for (const auto& nt : type.second.namedIds)
                delete[] nt.second.data;
        }
        for (auto type : namedTypes)
        {
            for (const auto& nt : type.second.numberedIds)
                delete[] nt.second.data;
            for (const auto& nt : type.second.namedIds)
                delete[] nt.second.data;
        }
    }
    catch (std::bad_array_new_length) { /* don't care */ }
}
bool ResourceContainer::LoadFiles()
{
    bool rv = true;
    for (const auto& name : names)
    {
        if (!LoadFile(name))
        {
            std::cout << "Error loading resource file '" << name << "'" << std::endl;
            rv = false;
        }
    }
    return rv;
}
int ResourceContainer::GetId(unsigned short* hdrdata, int& i, int& id, std::wstring& name)
{
    if (hdrdata[i] == 0xffff)
    {
        i++;
        id = hdrdata[i++];
    }
    else
    {
        wchar_t buf[65535];
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
bool ResourceContainer::LoadFile(const std::string& name)
{
    std::fstream in(name, std::ios::in | std::ios::binary);
    if (!in.is_open())
        return false;
    unsigned char buf[sizeof(resourceHeader)];
    in.read((char*)buf, sizeof(resourceHeader));
    if (memcmp(buf, resourceHeader, sizeof(resourceHeader)))
        return false;
    while (1)
    {
        unsigned hdr[2];
        in.read((char*)hdr, sizeof(hdr));
        if (in.eof())
            break;
        unsigned short* hdrdata = new unsigned short[hdr[1] / 2];
        in.read((char*)hdrdata, hdr[1] - sizeof(hdr));
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
            i++;
        int lang = hdrdata[i + 3];
        i += 8;  // past postheader
        delete[] hdrdata;
        // past end of header?
        if (i * 2 + 8 > hdr[1])
            return false;
        unsigned char* data = new unsigned char[hdr[0]];
        in.read((char*)data, hdr[0]);
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
        InsertResource(typeId, std::move(type), nameId, std::move(name), data, hdr[0], lang);
    }
    return true;
}
void ResourceContainer::InsertResource(int typeId, std::wstring type, int nameId, std::wstring name, unsigned char* data,
                                       size_t len, int language)
{
    ResourceData d, n;
    n.sig = SIGNAT;
    n.data = data;
    n.length = len;
    n.language = language;
    if (type.empty())
    {
        if (name.empty())
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
        if (name.empty())
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
        Utils::Fatal("Cannot resolve multiple resources with same ID");
    }
}
