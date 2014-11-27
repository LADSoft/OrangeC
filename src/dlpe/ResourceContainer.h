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
#ifndef ResourceContainer_h
#define ResourceContainer_h

#include <string>
#include <map>
#include <deque>

class ResourceContainer
{
public:
    ResourceContainer() { }
    ~ResourceContainer();
    void AddFile(std::string name) { names.push_back(name); }
    bool LoadFiles() ;
        
    size_t size() { return names.size(); }
#define SIGNAT 0x494e4753 //'SG' + ('NI' << 16)
    struct ResourceData
    {
        unsigned sig;
        unsigned char *data;
        int length;
        int language;
    };
    typedef std::map<std::wstring, ResourceData> NamedIds;
    typedef std::map<int, ResourceData> NumberedIds;
    struct Ids
    {
        NamedIds namedIds;
        NumberedIds numberedIds;
    };
    typedef std::map<std::wstring, Ids> NamedTypes;
    typedef std::map<int, Ids> NumberedTypes;
    NamedTypes namedTypes;
    NumberedTypes numberedTypes;
protected:
    int GetId(unsigned short *hdrdata, int &i, int &id, std::wstring &name);
    bool LoadFile(const std::string &name);
    void InsertResource(int typeId, std::wstring type, int nameId, std::wstring name, unsigned char *data, size_t len, int language);
    std::deque<std::string> names;
    static unsigned char resourceHeader[32];
};
#endif
