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

#include "PreProcessor.h"
#include "RCFile.h"
#include "ResFile.h"
#include "ResourceId.h"
#include "ResourceInfo.h"
#include "Accelerators.h"
#include "Bitmap.h"
#include "Cursor.h"
#include "Dialog.h"
#include "DlgInclude.h"
#include "FileHeader.h"
#include "Font.h"
#include "GenericResource.h"
#include "Icon.h"
#include "Menu.h"
#include "MessageTable.h"
#include "RCData.h"
#include "StringTable.h"
#include "VersionInfo.h"
#include "Utils.h"
#include <iostream>

#include <stdexcept>

std::string RCFile::CvtString(const std::wstring& str)
{
    char buf[1024], *q = buf;
    const wchar_t* p = str.c_str();
    while (*p)
        *q++ = *p++;
    *q = 0;
    return buf;
}
std::wstring RCFile::CvtString(const std::string& str)
{
    wchar_t buf[1024], *q = buf;
    const char* p = str.c_str();
    while (*p)
        *q++ = *p++;
    *q = 0;
    return buf;
}
bool RCFile::IsKeyword()
{
    bool rv = GetToken() && GetToken()->IsKeyword();
    return rv;
}
kw RCFile::GetTokenId()
{
    kw rv = (kw)0;
    if (IsKeyword())
    {
        rv = GetToken()->GetKeyword();
        NextToken();
    }
    return rv;
}
bool RCFile::IsNumber()
{
    bool rv = GetToken() &&
              (GetToken()->IsNumeric() ||
               (GetToken()->IsKeyword() && (GetToken()->GetKeyword() == kw::openpa || GetToken()->GetKeyword() == kw::plus ||
                                            GetToken()->GetKeyword() == kw::minus || GetToken()->GetKeyword() == kw::lnot ||
                                            GetToken()->GetKeyword() == kw::bcompl || GetToken()->GetKeyword() == kw::comma)));
    return rv;
}
unsigned RCFile::GetNumber()
{
    if (GetToken()->GetKeyword() == kw::comma)
        return 0;
    return expr.Eval();
}
bool RCFile::IsString()
{
    bool rv = GetToken() && GetToken()->IsString();
    return rv;
}
std::wstring RCFile::GetString()
{
    std::wstring rv;
    if (IsString())
    {
        while (IsString())
        {
            rv += GetToken()->GetString();
            NextToken();
        }
    }
    else
    {
        throw std::runtime_error("String expected");
    }
    return rv;
}
bool RCFile::IsIdentifier()
{
    bool rv = GetToken() && GetToken()->IsIdentifier();
    return rv;
}
std::wstring RCFile::GetId()
{
    std::wstring rv;
    if (IsIdentifier())
    {
        rv = CvtString(GetToken()->GetId());
        for (int i = 0; i < rv.size(); i++)
            rv[i] = toupper(rv[i]);
        NextToken();
    }
    else
    {
        throw std::runtime_error("identifier expected");
    }
    return rv;
}
void RCFile::NeedEol()
{
    if (!AtEol())
        throw std::runtime_error("End of line expected");
}
void RCFile::SkipComma()
{
    if (IsKeyword() && GetToken()->GetKeyword() == kw::comma)
        NextToken();
}
void RCFile::NeedBegin()
{
    if (!IsKeyword() || (GetToken()->GetKeyword() != kw::openbr && GetToken()->GetKeyword() != kw::BEGIN))
    {
        throw std::runtime_error("Begin expected");
    }
    NextToken();
}
void RCFile::NeedEnd()
{
    if (!IsKeyword() || (GetToken()->GetKeyword() != kw::closebr && GetToken()->GetKeyword() != kw::END))
        throw std::runtime_error("End expected");
    NextToken();
}
std::string RCFile::GetFileName()
{
    if (GetToken()->IsString())
    {
        std::string rv = CvtString(GetToken()->GetRawString());
        NextToken();
        rv = Utils::SearchForFile(includePath, rv);
        return rv;
    }
    else
    {
        std::string line = lexer.GetRestOfLine();
        size_t s = line.find_first_not_of(" \t\v");
        if (s == std::string::npos)
            throw std::runtime_error("Expected valid file name");
        size_t e = line.find_first_of(" \t\v", s);
        if (e == std::string::npos)
            e = line.size();
        std::string rv = line.substr(s, e - s);
        line.erase(0, e);
        lexer.Reset(line);
        rv = Utils::SearchForFile(includePath, rv);
        return rv;
    }
}
void RCFile::SkimStructOrEnum()
{
    while (GetToken()->GetKeyword() != kw::semi && GetToken()->GetKeyword() != kw::BEGIN && !AtEof())
        NextToken();
    if (GetToken()->GetKeyword() == kw::semi)
    {
        NextToken();
        return;
    }

    NeedBegin();

    int count = 0;
    while ((count > 0 || GetToken()->GetKeyword() != kw::END) && !AtEof())
    {
        switch (GetToken()->GetKeyword())
        {
            case kw::BEGIN:
                count++;
                break;
            case kw::END:
                count--;
                break;
        }
        if (count < 0)
            break;
        NextToken();
    }
    NeedEnd();
    if (GetToken()->IsIdentifier())
        NextToken();
    if (GetToken()->IsKeyword() && GetToken()->GetKeyword() == kw::semi)
        NextToken();
    else
        throw std::runtime_error("semicolon expected");
}
void RCFile::SkimTypedef()
{
    if (GetToken()->GetId() == "struct" || GetToken()->GetId() == "enum" || GetToken()->GetId() == "union")
    {
        NextToken();
        SkimStructOrEnum();
    }
    else
    {
        while (GetToken()->GetKeyword() != kw::semi && !AtEof())
        {
            NextToken();
        }
        if (GetToken()->IsKeyword() && GetToken()->GetKeyword() == kw::semi)
            NextToken();
        else
            throw std::runtime_error("semicolon expected");
    }
}
bool RCFile::IsGenericResource()
{
    kw n = GetToken()->GetKeyword();
    if (GetToken()->IsString())
        return true;
    switch (n)
    {
        case kw::BEGIN:
        case kw::DISCARDABLE:
        case kw::PURE:
        case kw::PRELOAD:
        case kw::MOVEABLE:
        case kw::LOADONCALL:
        case kw::NONDISCARDABLE:
        case kw::IMPURE:
        case kw::FIXED:
        case kw::LANGUAGE:
        case kw::VERSION:
        case kw::CHARACTERISTICS:
            return true;
        default:
            return false;
    }
}
void RCFile::SkimPrototype()
{
    int count = 0;
    while (GetToken()->GetKeyword() != kw::openpa && !AtEof())
    {
        NextToken();
    }
    if (GetToken()->IsKeyword() && GetToken()->GetKeyword() == kw::openpa)
        NextToken();
    else
        throw std::runtime_error("open paren expected");
    while ((count > 0 || GetToken()->GetKeyword() != kw::closepa) && !AtEof())
    {
        switch (GetToken()->GetKeyword())
        {
            case kw::openpa:
                count++;
                break;
            case kw::closepa:
                count--;
                break;
        }
        if (count < 0)
            break;
        NextToken();
    }
    if (GetToken()->IsKeyword() && GetToken()->GetKeyword() == kw::closepa)
        NextToken();
    else
        throw std::runtime_error("close parenthesis expected");
    if (GetToken()->IsKeyword() && GetToken()->GetKeyword() == kw::semi)
        NextToken();
    else
        throw std::runtime_error("semicolon expected");
}
Resource* RCFile::GetRes()
{
    kw type;
    int val = 0;
    std::wstring name;
    ResourceId id;
    bool done = false;
    Resource* rv = nullptr;
    while (!done && !AtEof())
    {
        done = true;
        if (!IsNumber() && GetToken()->IsKeyword())
        {
            type = GetToken()->GetKeyword();
            name = GetToken()->GetString();
            if (type >= kw::ACCELERATORS)
            {
                if (type != kw::STRINGTABLE && type != kw::LANGUAGE)
                {
                    NextToken();
                    if (GetToken()->IsKeyword() && GetToken()->GetKeyword() >= kw::ACCELERATORS)
                    {
                        id.SetName(name);
                        type = GetToken()->GetKeyword();
                        name = GetToken()->GetString();
                    }
                    else if (GetToken()->IsIdentifier())
                    {
                        id.SetName(name);
                        type = (kw)-1;
                        name = CvtString(GetToken()->GetId());
                    }
                    else if (IsNumber())
                    {
                        type = (kw)-2;
                        val = GetNumber();
                    }
                    else
                    {
                        throw std::runtime_error("Expected resource type");
                    }
                    if (type != (kw)-2)
                        NextToken();
                }
                else
                {
                    NextToken();
                }
            }
            else
            {
                throw std::runtime_error("Expected resource identifier");
            }
        }
        else
        {
            id.ReadRC(*this);
            if (id.IsNamed())
            {
                std::wstring firstToken = id.GetName();
                if (firstToken == L"STRUCT" || firstToken == L"ENUM" || firstToken == L"UNION")
                {
                    SkimStructOrEnum();
                    done = false;
                    continue;
                }
                else if (firstToken == L"TYPEDEF")
                {
                    SkimTypedef();
                    done = false;
                    continue;
                }
            }
            if (GetToken()->IsKeyword())
            {
                type = GetToken()->GetKeyword();
                name = GetToken()->GetString();
            }
            else if (GetToken()->IsIdentifier())
            {
                type = (kw)-1;
                name = CvtString(GetToken()->GetId());
            }
            else if (IsNumber())
            {
                type = (kw)-2;
                val = GetNumber();
            }
            else
            {
                throw std::runtime_error("Expected resource type");
            }
            if (type != (kw)-2)
                NextToken();
        }
        ResourceInfo info(language);
        info.SetFlags(ResourceInfo::Moveable | ResourceInfo::Discardable);
        rv = nullptr;
        switch (type)
        {
            case (kw)-1:
                for (int i = 0; i < name.size(); i++)
                    name[i] = toupper(name[i]);
                if (!id.IsNamed() || IsGenericResource())
                    rv = new GenericResource(ResourceId(name), id, info);
                else if (name == L"TYPEDEF")
                {
                    SkimTypedef();
                    done = false;
                    continue;
                }
                else if (name == L"STRUCT" || name == L"UNION" || name == L"ENUM")
                {
                    SkimStructOrEnum();
                    done = false;
                    continue;
                }
                else  // assumeprototype
                {
                    SkimPrototype();
                    done = false;
                    continue;
                }
                break;
            case (kw)-2:
                rv = new GenericResource(ResourceId(val), id, info);
                break;
            case kw::ACCELERATORS:
                rv = new Accelerators(id, info);
                break;
            case kw::TBITMAP:
                rv = new Bitmap(id, info);
                break;
            case kw::CURSOR:
                rv = new GroupCursor(id, info);
                break;
            case kw::DIALOG:
                rv = new Dialog(id, info, false);
                break;
            case kw::DIALOGEX:
                rv = new Dialog(id, info, true);
                break;
            case kw::DLGINCLUDE:
                rv = new DlgInclude(id, info);
                break;
            case kw::FONT:
                rv = new Font(id, info);
                break;
            case kw::ICON:
                rv = new GroupIcon(id, info);
                break;
            case kw::MENU:
                rv = new Menu(id, info, false);
                break;
            case kw::MENUEX:
                rv = new Menu(id, info, true);
                break;
            case kw::RCDATA:
                rv = new RCData(id, info);
                break;
            case kw::VERSIONINFO:
                rv = new VersionInfo(id, info);
                break;
            case kw::MESSAGETABLE:
                rv = new MessageTable(id, info);
                break;
            case kw::STRINGTABLE: {
                std::unique_ptr<StringTable> temp = std::make_unique<StringTable>(info);
                temp->ReadRC(*this);
                rv = nullptr;
                done = false;
            }
            break;
            case kw::LANGUAGE: {
                language = GetNumber();
                SkipComma();
                language |= GetNumber() << 10;
                NeedEol();
                done = false;
                break;
            }
            case kw::RCINCLUDE: {
                std::string name = GetFileName();
                pp.IncludeFile(name);
            }
            break;
            default:
                throw std::runtime_error("Invalid resource type");
                break;
        }
        if (rv)
            rv->ReadRC(*this);
    }
    return rv;
}
bool RCFile::Read()
{
    resFile.Reset();
    resFile.Add(new FileHeader());  // add the 32 byte header into the resource database

    Resource* res;
    try
    {
        while (!lexer.AtEof() && (res = GetRes()))
            resFile.Add(res);
    }
    catch (std::runtime_error& e)
    {
        Errors::Error(e.what());
        return false;
    }
    return true;
}
