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
#include "VersionInfo.h"
#include "ResFile.h"
#include "RCFile.h"
#include <windows.h>
#include <exception>

void StringVerInfo::WriteRes(ResFile &resFile)
{
    resFile.Align();
    size_t pos = resFile.GetPos();
    resFile.WriteWord(0);
    resFile.WriteWord(0);
    resFile.WriteWord(0);
    resFile.WriteString(L"StringFileInfo");
    resFile.Align();
    size_t pos1 = resFile.GetPos();
    resFile.WriteWord(0);
    resFile.WriteWord(0);
    resFile.WriteWord(0);
    resFile.WriteString(language);
    for (iterator it = begin(); it != end(); ++it)
    {
        resFile.Align();
        size_t pos3 = resFile.GetPos();
        resFile.WriteWord(0);
        resFile.WriteWord(it->value.size());
        resFile.WriteWord(1);
        resFile.WriteString(it->key);
        resFile.Align();
        int n = it->value.size();
        // write word by word for portability
        for (int i=0; i < n; i++)
            resFile.WriteWord(it->value[i]);
        size_t pos4 = resFile.GetPos();
        resFile.SetPos(pos3);
        resFile.WriteWord(pos4-pos3);
        resFile.SetPos(pos4);
    }
    size_t pos2 = resFile.GetPos();
    resFile.SetPos(pos1);
    resFile.WriteWord(pos2 - pos1);
    resFile.SetPos(pos);
    resFile.WriteWord(pos2 - pos);
    resFile.SetPos(pos2);
}
void StringVerInfo::ReadRC(RCFile &rcFile)
{
    rcFile.NeedBegin();
    while (rcFile.GetToken()->GetKeyword() == Lexer::VALUE)
    {
        rcFile.NextToken();
        std::wstring key = rcFile.GetString();
        rcFile.SkipComma();
        std::wstring value = rcFile.GetString();
        while (rcFile.IsString())
            value += rcFile.GetString();
        rcFile.NeedEol();
        Add(key, value);
    }
    rcFile.NeedEnd();
}
VarVerInfo::Info::Info(const Info &Old)
{
    key = Old.key;
    for (const_iterator it = Old.begin(); it != Old.end(); ++it)
        languages.push_back(*it);
}
void VarVerInfo::WriteRes(ResFile &resFile)
{
    resFile.Align();
    size_t pos = resFile.GetPos();
    resFile.WriteWord(0);
    resFile.WriteWord(0);
    resFile.WriteWord(0);
    resFile.WriteString(L"VarFileInfo");
    for (iterator it = info.begin(); it != info.end(); ++it)
    {
        resFile.Align();
        size_t pos3 = resFile.GetPos();
        resFile.WriteWord(0);
        resFile.WriteWord(it->languages.size() * sizeof(WORD));
        resFile.WriteWord(0);
        resFile.WriteString(it->key);
        resFile.Align();
        for (Info::iterator it1 = it->begin(); it1 != it->end(); ++ it1)
        {
            resFile.WriteWord(*it1);
        }
        size_t pos4 = resFile.GetPos();
        resFile.SetPos(pos3);
        resFile.WriteWord(pos4 - pos3);
        resFile.SetPos(pos4);
    }
    size_t pos1 = resFile.GetPos();
    resFile.SetPos(pos);
    resFile.WriteWord(pos1 - pos);
    resFile.SetPos(pos1);
}
void VarVerInfo::ReadRC(RCFile &rcFile)
{
    while (rcFile.GetToken()->GetKeyword() == Lexer::VALUE)
    {
        rcFile.NextToken();
        
        std::wstring key = rcFile.GetString();
        if (rcFile.GetToken()->GetKeyword() != Lexer::comma)
            throw new std::runtime_error("Comma expected");
        Info v(key);
        while (rcFile.GetToken()->GetKeyword() == Lexer::comma)
        {
            rcFile.NextToken();
            v.languages.push_back( rcFile.GetNumber());
        }
        info.push_back(v);
    }
    rcFile.NeedEol();
}
VersionInfo::~VersionInfo()
{
    for (std::deque<InternalVerInfo *>::iterator it = varInfo.begin();
         it != varInfo.end(); ++it)
    {
        InternalVerInfo *p = *it;
        delete p;
    }
    varInfo.clear();
}
void VersionInfo::WriteRes(ResFile &resFile)
{
    Resource::WriteRes(resFile);
    size_t pos = resFile.GetPos();
    resFile.WriteWord(0); // length goes here
    if (fixed)
    {
        resFile.WriteWord(52);
    }
    else
    {
        resFile.WriteWord(0);
    }
    resFile.WriteWord(0);
    resFile.WriteString(L"VS_VERSION_INFO");
    resFile.Align();
    if (fixed)
    {
        resFile.WriteDWord(VS_FFI_SIGNATURE);
        resFile.WriteDWord(VS_FFI_STRUCVERSION);
        resFile.WriteDWord(fileVersionMS);
        resFile.WriteDWord(fileVersionLS);
        resFile.WriteDWord(productVersionMS);
        resFile.WriteDWord(productVersionLS);
        resFile.WriteDWord(fileFlagsMask);
        resFile.WriteDWord(fileFlags);
        resFile.WriteDWord(fileOS);
        resFile.WriteDWord(fileType);
        resFile.WriteDWord(fileSubType);
        resFile.WriteDWord(fileDateMS);
        resFile.WriteDWord(fileDateLS);
    }
    for (iterator it = begin(); it != end(); ++it)
    {
        (*it)->WriteRes(resFile);
    }
    size_t pos1 = resFile.GetPos();
    resFile.SetPos(pos);
    resFile.WriteWord(pos1 - pos);
    resFile.SetPos(pos1);
    resFile.Release();
}
void VersionInfo::ReadRC(RCFile &rcFile)
{
    resInfo.SetFlags(ResourceInfo::Pure | ResourceInfo::Moveable);
    bool done = false;
    int count = 0;
    while (!done)
    {
        switch (rcFile.GetTokenId())
        {
            case Lexer::FILEVERSION:
                {
                    count ++;
                    fileVersionMS = rcFile.GetNumber() << 16;
                    if (!rcFile.AtEol())
                    {
                        rcFile.SkipComma();
                        fileVersionMS |= rcFile.GetNumber() &0xffff;
                        if (!rcFile.AtEol())
                        {
                            rcFile.SkipComma();
                            fileVersionLS = rcFile.GetNumber() << 16;
                            if (!rcFile.AtEol())
                            {
                                rcFile.SkipComma();
                                fileVersionLS |= rcFile.GetNumber() &0xffff;
                            }
                        }
                    }
                    rcFile.NeedEol();
                }
                break;
            case Lexer::PRODUCTVERSION:
                {
                    count ++;
                    productVersionMS = rcFile.GetNumber() << 16;
                    if (!rcFile.AtEol())
                    {
                        rcFile.SkipComma();
                        productVersionMS |= rcFile.GetNumber() &0xffff;
                        if (!rcFile.AtEol())
                        {
                            rcFile.SkipComma();
                            productVersionLS = rcFile.GetNumber() << 16;
                            if (!rcFile.AtEol())
                            {
                                rcFile.SkipComma();
                                productVersionLS |= rcFile.GetNumber() &0xffff;
                            }
                        }
                    }
                    rcFile.NeedEol();
                }
                break;
            case Lexer::FILEFLAGSMASK:
                count++;
                fileFlagsMask = rcFile.GetNumber();
                rcFile.NeedEol();
                break;
            case Lexer::FILEFLAGS:
                count++;
                fileFlags = rcFile.GetNumber();
                rcFile.NeedEol();
                break;
            case Lexer::FILEOS:
                count++;
                fileOS = rcFile.GetNumber();
                rcFile.NeedEol();
                break;
            case Lexer::FILETYPE:
                count++;
                fileType = rcFile.GetNumber();
                rcFile.NeedEol();
                break;
            case Lexer::FILESUBTYPE:
                count++;
                fileSubType = rcFile.GetNumber();
                rcFile.NeedEol();
                break;
            case Lexer::BEGIN:
                done = true;
                if (!count)
                    throw new std::runtime_error("Version info expected");
                fixed = true;
                break;
            default:
                throw new std::runtime_error("Begin Expected");
                break;
        }
    }
    count = 0;
    while (rcFile.GetToken()->GetKeyword() == Lexer::BLOCK)
    {
        rcFile.NextToken();
        count++;
        std::wstring type = rcFile.GetString();
        if (type == L"StringFileInfo")
        {
            rcFile.NeedEol();
            rcFile.NeedBegin();
            if (rcFile.GetTokenId() != Lexer::BLOCK)
                throw new std::runtime_error("Block expected");
            std::wstring language = rcFile.GetString();
            StringVerInfo *v = new StringVerInfo(language);
            varInfo.push_back(v);
            rcFile.NeedEol();
            v->ReadRC(rcFile);
            rcFile.NeedEnd();
        }
        else if (type == L"VarFileInfo")
        {
            rcFile.NeedEol();
            rcFile.NeedBegin();
            VarVerInfo *v = new VarVerInfo();
            v->ReadRC(rcFile);
            varInfo.push_back(v);
            rcFile.NeedEnd();
        }
        else
            throw new std::runtime_error("Invalid version info type");

    }
    rcFile.NeedEnd();
}
