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
#include <windows.h>
#include <iostream>

#include <exception>

std::string RCFile::CvtString(const std::wstring &str)
{
    char buf[1024], *q = buf;
    const WCHAR *p = str.c_str();
    while (*p)
        *q++ = *p++;
    *q = 0;
    return buf;
}
std::wstring RCFile::CvtString(const std::string &str)
{
    WCHAR buf[1024], *q = buf;
    const char *p = str.c_str();
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
    bool rv = GetToken() && (GetToken()->IsNumeric() || (GetToken()->IsKeyword() && (
        GetToken()->GetKeyword() == Lexer::openpa || GetToken()->GetKeyword() == Lexer::plus ||
        GetToken()->GetKeyword() == Lexer::minus || GetToken()->GetKeyword() == Lexer::not ||
        GetToken()->GetKeyword() == Lexer::compl)||GetToken()->GetKeyword() == Lexer::comma));
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
        for (int i=0; i < rv.size(); i++)
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
    if (!IsKeyword() || GetToken()->GetKeyword() != Lexer::openbr && GetToken()->GetKeyword() != Lexer::BEGIN)
{
        throw new std::runtime_error("Begin expected");
}
    NextToken();
}
void RCFile::NeedEnd()
{
    if (!IsKeyword() || GetToken()->GetKeyword() != Lexer::closebr && GetToken()->GetKeyword() != Lexer::END)
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
        std::string rv = line.substr(s, e-s);
        line.erase(0, e);
        lexer.Reset(line);
        rv = Utils::SearchForFile(includePath, rv);
        return rv;
    }
}
Resource *RCFile::GetRes()
{
    int type;
    int val = 0;
    std::wstring name;
    ResourceId id;
    bool done = false;
    Resource *rv = NULL;
    while (!done && !AtEof())
    {
        done= true;
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
                        name =CvtString(GetToken()->GetId());
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
        rv = NULL;
        switch (type)
        {
            case -1:
                for (int i=0 ; i < name.size(); i++)
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
                rv = new StringTable(info);
                rv->ReadRC(*this);
                delete rv;
                rv = NULL;
                done = false;
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
    resFile.Add(new FileHeader()); // add the 32 byte header into the resource database
    
    Resource *res;
    try
    {
        while (!lexer.AtEof() && (res = GetRes()))
            resFile.Add(res);
    }
    catch (std::runtime_error *e)
    {
        Errors::Error(e->what());
        delete e;
        return false;
    }
    return true;
}
