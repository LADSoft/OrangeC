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

#include "VersionInfo.h"
#include "ResFile.h"
#include "RCFile.h"
#ifndef HAVE_UNISTD_H
#    include <windows.h>
#endif
#include <stdexcept>

void StringVerInfo::WriteRes(ResFile& resFile)
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
    for (auto info : *this)
    {
        resFile.Align();
        size_t pos3 = resFile.GetPos();
        resFile.WriteWord(0);
        resFile.WriteWord(info.value.size());
        resFile.WriteWord(1);
        resFile.WriteString(info.key);
        resFile.Align();
        int n = info.value.size();
        // write word by word for portability
        for (int i = 0; i < n; i++)
            resFile.WriteWord(info.value[i]);
        size_t pos4 = resFile.GetPos();
        resFile.SetPos(pos3);
        resFile.WriteWord(pos4 - pos3);
        resFile.SetPos(pos4);
    }
    size_t pos2 = resFile.GetPos();
    resFile.SetPos(pos1);
    resFile.WriteWord(pos2 - pos1);
    resFile.SetPos(pos);
    resFile.WriteWord(pos2 - pos);
    resFile.SetPos(pos2);
}
void StringVerInfo::ReadRC(RCFile& rcFile)
{
    rcFile.NeedBegin();
    while (rcFile.GetToken()->GetKeyword() == kw::VALUE)
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
VarVerInfo::Info::Info(const Info& Old)
{
    key = Old.key;
    for (auto info : Old)
        languages.push_back(info);
}
void VarVerInfo::WriteRes(ResFile& resFile)
{
    resFile.Align();
    size_t pos = resFile.GetPos();
    resFile.WriteWord(0);
    resFile.WriteWord(0);
    resFile.WriteWord(0);
    resFile.WriteString(L"VarFileInfo");
    for (auto item : info)
    {
        resFile.Align();
        size_t pos3 = resFile.GetPos();
        resFile.WriteWord(0);
        resFile.WriteWord(item.languages.size() * sizeof(wchar_t));
        resFile.WriteWord(0);
        resFile.WriteString(item.key);
        resFile.Align();
        for (auto val : item)
        {
            resFile.WriteWord(val);
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
void VarVerInfo::ReadRC(RCFile& rcFile)
{
    while (rcFile.GetToken()->GetKeyword() == kw::VALUE)
    {
        rcFile.NextToken();

        std::wstring key = rcFile.GetString();
        if (rcFile.GetToken()->GetKeyword() != kw::comma)
            throw std::runtime_error("Comma expected");
        Info v(key);
        while (rcFile.GetToken()->GetKeyword() == kw::comma)
        {
            rcFile.NextToken();
            v.languages.push_back(rcFile.GetNumber());
        }
        info.push_back(v);
    }
    rcFile.NeedEol();
}
VersionInfo::~VersionInfo() {}
void VersionInfo::Add(InternalVerInfo* v)
{
    std::unique_ptr<InternalVerInfo> temp(v);
    varInfo.push_back(std::move(temp));
}
void VersionInfo::WriteRes(ResFile& resFile)
{
    Resource::WriteRes(resFile);
    size_t pos = resFile.GetPos();
    resFile.WriteWord(0);  // length goes here
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
#ifndef HAVE_UNISTD_H
        resFile.WriteDWord(VS_FFI_SIGNATURE);
        resFile.WriteDWord(VS_FFI_STRUCVERSION);
#else
        resFile.WriteDWord(0);
        resFile.WriteDWord(0);
#endif
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
    for (auto& res : *this)
    {
        res->WriteRes(resFile);
    }
    size_t pos1 = resFile.GetPos();
    resFile.SetPos(pos);
    resFile.WriteWord(pos1 - pos);
    resFile.SetPos(pos1);
    resFile.Release();
}
void VersionInfo::ReadRC(RCFile& rcFile)
{
    resInfo.SetFlags(ResourceInfo::Pure | ResourceInfo::Moveable);
    bool done = false;
    int count = 0;
    while (!done)
    {
        switch (rcFile.GetTokenId())
        {
            case kw::FILEVERSION: {
                count++;
                fileVersionMS = rcFile.GetNumber() << 16;
                if (!rcFile.AtEol())
                {
                    rcFile.SkipComma();
                    fileVersionMS |= rcFile.GetNumber() & 0xffff;
                    if (!rcFile.AtEol())
                    {
                        rcFile.SkipComma();
                        fileVersionLS = rcFile.GetNumber() << 16;
                        if (!rcFile.AtEol())
                        {
                            rcFile.SkipComma();
                            fileVersionLS |= rcFile.GetNumber() & 0xffff;
                        }
                    }
                }
                rcFile.NeedEol();
            }
            break;
            case kw::PRODUCTVERSION: {
                count++;
                productVersionMS = rcFile.GetNumber() << 16;
                if (!rcFile.AtEol())
                {
                    rcFile.SkipComma();
                    productVersionMS |= rcFile.GetNumber() & 0xffff;
                    if (!rcFile.AtEol())
                    {
                        rcFile.SkipComma();
                        productVersionLS = rcFile.GetNumber() << 16;
                        if (!rcFile.AtEol())
                        {
                            rcFile.SkipComma();
                            productVersionLS |= rcFile.GetNumber() & 0xffff;
                        }
                    }
                }
                rcFile.NeedEol();
            }
            break;
            case kw::FILEFLAGSMASK:
                count++;
                fileFlagsMask = rcFile.GetNumber();
                rcFile.NeedEol();
                break;
            case kw::FILEFLAGS:
                count++;
                fileFlags = rcFile.GetNumber();
                rcFile.NeedEol();
                break;
            case kw::FILEOS:
                count++;
                fileOS = rcFile.GetNumber();
                rcFile.NeedEol();
                break;
            case kw::FILETYPE:
                count++;
                fileType = rcFile.GetNumber();
                rcFile.NeedEol();
                break;
            case kw::FILESUBTYPE:
                count++;
                fileSubType = rcFile.GetNumber();
                rcFile.NeedEol();
                break;
            case kw::BEGIN:
                done = true;
                if (!count)
                    throw std::runtime_error("Version info expected");
                fixed = true;
                break;
            default:
                throw std::runtime_error("Begin Expected");
                break;
        }
    }
    count = 0;
    while (rcFile.GetToken()->GetKeyword() == kw::BLOCK)
    {
        rcFile.NextToken();
        count++;
        std::wstring type = rcFile.GetString();
        if (type == L"StringFileInfo")
        {
            rcFile.NeedEol();
            rcFile.NeedBegin();
            if (rcFile.GetTokenId() != kw::BLOCK)
                throw std::runtime_error("Block expected");
            std::wstring language = rcFile.GetString();
            varInfo.push_back(std::make_unique<StringVerInfo>(language));
            rcFile.NeedEol();
            varInfo.back()->ReadRC(rcFile);
            rcFile.NeedEnd();
        }
        else if (type == L"VarFileInfo")
        {
            rcFile.NeedEol();
            rcFile.NeedBegin();
            varInfo.push_back(std::make_unique<VarVerInfo>());
            varInfo.back()->ReadRC(rcFile);
            rcFile.NeedEnd();
        }
        else
            throw std::runtime_error("Invalid version info type");
    }
    rcFile.NeedEnd();
}
