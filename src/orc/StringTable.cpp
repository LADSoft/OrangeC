/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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
 */

#include "StringTable.h"
#include "ResFile.h"
#include "RCFile.h"
#include "Errors.h"
#include <stdexcept>

std::map<int, StringTable*> StringTable::stringTables;
ResourceInfo StringTable::currentInfo(0);

StringTable::StringTable(const ResourceInfo& info) : loadedInfo(info), Resource(eString, 0, info) {}
StringTable::StringTable(int Id, const ResourceInfo& info) : Resource(eString, ResourceId((Id >> 4) + 1), info), loadedInfo(0)
{
    stringTables[(Id >> 4) + 1] = this;
}
void StringTable::WriteRes(ResFile& resFile)
{
    Resource::WriteRes(resFile);
    for (int i = 0; i < 16; i++)
    {
        int n = strings[i].size();
        if (n)
        {
            // these are counted strings, not null terminated ones
            resFile.WriteWord(strings[i].size());
            // writes word by word for portability
            for (int j = 0; j < n; j++)
                resFile.WriteWord(strings[i][j]);
        }
        else
            resFile.WriteWord(0);
    }
    resFile.Release();
}
void StringTable::ReadRC(RCFile& rcFile)
{
    currentInfo = resInfo;
    currentInfo.SetFlags(resInfo.GetFlags() | ResourceInfo::Pure);
    currentInfo.ReadRC(rcFile, false);
    rcFile.NeedEol();
    rcFile.NeedBegin();
    while (rcFile.IsNumber())
    {
        int id = rcFile.GetNumber();
        rcFile.SkipComma();
        std::wstring str = rcFile.GetString();
        StringTable* current = Lookup(rcFile.GetResFile(), id);
        current->Add(id, str);
        rcFile.NeedEol();
    }
    rcFile.NeedEnd();
}

void StringTable::Add(int id, const std::wstring& val)
{
    int errid = id;
    id &= 15;
    if (!strings[id].empty())
        throw std::runtime_error(std::string("String id ") + Errors::ToNum(errid) + " already used");
    strings[id] = val;
}
StringTable* StringTable::Lookup(ResFile& resFile, int id)
{
    int idv = (id >> 4) + 1;
    auto it = stringTables.find(idv);
    if (it != stringTables.end())
    {
        return it->second;
    }
    StringTable* rv = new StringTable(id, currentInfo);
    // if (rv)
    resFile.Add(rv);
    return rv;
}
