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
#include "StringTable.h"
#include "ResFile.h"
#include "RCFile.h"
#include "Errors.h"
#include <windows.h>
#include <exception>

std::map<int, StringTable *> StringTable::stringTables;
ResourceInfo StringTable::currentInfo(0);

StringTable::StringTable(const ResourceInfo &info) 
    : loadedInfo(info) , Resource(eString, 0, info)
{ 
}
StringTable::StringTable(int Id, const ResourceInfo &info)
    : Resource(eString, ResourceId((Id >> 4) + 1), info) , loadedInfo(0)
{ 
    stringTables[(Id >> 4) + 1]  =this; 
}
void StringTable::WriteRes(ResFile &resFile)
{
    Resource::WriteRes(resFile);
    for (int i=0; i < 16; i++)
    {
        int n = strings[i].size();
        if (n)
        {
            // these are counted strings, not null terminated ones
            resFile.WriteWord(strings[i].size());
            // writes word by word for portability
            for (int j=0; j < n; j++)
                resFile.WriteWord(strings[i][j]);
        }
        else
            resFile.WriteWord(0);
    }
    resFile.Release();
}
bool StringTable::ReadRC(RCFile &rcFile)
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
        StringTable *current = Lookup(rcFile.GetResFile(), id);
        current->Add(id, str);
        rcFile.NeedEol();
    }
    rcFile.NeedEnd();
    return true;
}

void StringTable::Add(int id, std::wstring val)
{
    int errid = id;
    id &= 15;
    if (strings[id].size() != 0)
        throw new std::runtime_error(std::string("String id ") + Errors::ToNum(errid) + " already used");
    strings[id] = val;
}
StringTable *StringTable::Lookup(ResFile &resFile, int id)
{
    int idv = (id >> 4) +  1;
    std::map<int, StringTable *>::iterator it = stringTables.find(idv);
    if (it != stringTables.end())
    {
        return it->second;
    }
    else
    {	
        StringTable *rv = new StringTable(id, currentInfo);
        if (rv)
            resFile.Add(rv);
        return rv;
    }
}
