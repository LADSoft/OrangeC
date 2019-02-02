/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
unsigned RCFile::GetTokenId()
{
    unsigned rv = 0;
    if (IsKeyword())
    {
        rv = GetToken()->GetKeyword();
        NextToken();
    }
    return rv;
}
bool RCFile::IsNumber()
{
    bool rv =
        GetToken() &&
        (GetToken()->IsNumeric() ||
         (GetToken()->IsKeyword() && (GetToken()->GetKeyword() == Lexer::openpa || GetToken()->GetKeyword() == Lexer::plus ||
                                      GetToken()->GetKeyword() == Lexer::minus || GetToken()->GetKeyword() == Lexer::lnot ||
                                      GetToken()->GetKeyword() == Lexer::bcompl || GetToken()->GetKeyword() == Lexer::comma)));
    return rv;
}
unsigned RCFile::GetNumber()
{
    if (GetToken()->GetKeyword() == Lexer::comma)
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
        throw new std::runtime_error("String expected");
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
        throw new std::runtime_error("identifier expected");
    }
    return rv;
}
void RCFile::NeedEol()
{
    if (!AtEol())
        throw new std::runtime_error("End of line expected");
}
void RCFile::SkipComma()
{
    if (IsKeyword() && GetToken()->GetKeyword() == Lexer::comma)
        NextToken();
}
void RCFile::NeedBegin()
{
    if (!IsKeyword() || (GetToken()->GetKeyword() != Lexer::openbr && GetToken()->GetKeyword() != Lexer::BEGIN))
    {
        throw new std::runtime_error("Begin expected");
    }
    NextToken();
}
void RCFile::NeedEnd()
{
    if (!IsKeyword() || (GetToken()->GetKeyword() != Lexer::closebr && GetToken()->GetKeyword() != Lexer::END))
        throw new std::runtime_error("End expected");
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
            throw new std::runtime_error("Expected valid file name");
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
Resource* RCFile::GetRes()
{
    int type;
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
            if (type >= Lexer::ACCELERATORS)
            {
                if (type != Lexer::STRINGTABLE && type != Lexer::LANGUAGE)
                {
                    NextToken();
                    if (GetToken()->IsKeyword() && GetToken()->GetKeyword() >= Lexer::ACCELERATORS)
                    {
                        id.SetName(name);
                        type = GetToken()->GetKeyword();
                        name = GetToken()->GetString();
                    }
                    else if (GetToken()->IsIdentifier())
                    {
                        id.SetName(name);
                        type = -1;
                        name = CvtString(GetToken()->GetId());
                    }
                    else if (IsNumber())
                    {
                        type = -2;
                        val = GetNumber();
                    }
                    else
                    {
                        throw new std::runtime_error("Expected resource type");
                    }
                    if (type != -2)
                        NextToken();
                }
                else
                {
                    NextToken();
                }
            }
            else
            {
                throw new std::runtime_error("Expected resource identifier");
            }
        }
        else
        {
            id.ReadRC(*this);
            if (GetToken()->IsKeyword())
            {
                type = GetToken()->GetKeyword();
                name = GetToken()->GetString();
            }
            else if (GetToken()->IsIdentifier())
            {
                type = -1;
                name = CvtString(GetToken()->GetId());
            }
            else if (IsNumber())
            {
                type = -2;
                val = GetNumber();
            }
            else
            {
                throw new std::runtime_error("Expected resource type");
            }
            if (type != -2)
                NextToken();
        }
        ResourceInfo info(language);
        info.SetFlags(ResourceInfo::Moveable | ResourceInfo::Discardable);
        rv = nullptr;
        switch (type)
        {
            case -1:
                for (int i = 0; i < name.size(); i++)
                    name[i] = toupper(name[i]);
                rv = new GenericResource(ResourceId(name), id, info);
                break;
            case -2:
                rv = new GenericResource(ResourceId(val), id, info);
                break;
            case Lexer::ACCELERATORS:
                rv = new Accelerators(id, info);
                break;
            case Lexer::TBITMAP:
                rv = new Bitmap(id, info);
                break;
            case Lexer::CURSOR:
                rv = new GroupCursor(id, info);
                break;
            case Lexer::DIALOG:
                rv = new Dialog(id, info, false);
                break;
            case Lexer::DIALOGEX:
                rv = new Dialog(id, info, true);
                break;
            case Lexer::DLGINCLUDE:
                rv = new DlgInclude(id, info);
                break;
            case Lexer::FONT:
                rv = new Font(id, info);
                break;
            case Lexer::ICON:
                rv = new GroupIcon(id, info);
                break;
            case Lexer::MENU:
                rv = new Menu(id, info, false);
                break;
            case Lexer::MENUEX:
                rv = new Menu(id, info, true);
                break;
            case Lexer::RCDATA:
                rv = new RCData(id, info);
                break;
            case Lexer::VERSIONINFO:
                rv = new VersionInfo(id, info);
                break;
            case Lexer::MESSAGETABLE:
                rv = new MessageTable(id, info);
                break;
            case Lexer::STRINGTABLE:
            {
                std::unique_ptr<StringTable> temp = std::make_unique<StringTable>(info);
                temp->ReadRC(*this);
                rv = nullptr;
                done = false;
            }
            break;
            case Lexer::LANGUAGE:
            {
                language = GetNumber();
                SkipComma();
                language |= GetNumber() << 10;
                NeedEol();
                done = false;
                break;
            }
            case Lexer::RCINCLUDE:
            {
                std::string name = GetFileName();
                pp.IncludeFile(name);
            }
            break;
            default:
                throw new std::runtime_error("Invalid resource type");
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
    catch (std::runtime_error* e)
    {
        Errors::Error(e->what());
        delete e;
        return false;
    }
    return true;
}
