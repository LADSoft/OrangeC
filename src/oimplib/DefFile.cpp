/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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
#include "ppExpr.h"
#include "DefFile.h"
#include "PEHeader.h"
#include "MZHeader.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "UTF8.h"

bool DefFile::initted;
KeywordHash DefFile::keywords;

#define SPACES " \t\v\f\r\n"
#define COMMA ","
bool IsSymbolStartChar(const char *data)
{
    return *data == '_' || *data == '$' || *data == '?' || *data == '.' || UTF8::IsAlpha(data);
}
bool IsSymbolChar(const char *data)
{
    return *data == '_' || *data == '$' || *data == '#' || *data == '@' || *data == '~' ||
        *data == '?' || *data == '.' || UTF8::IsAlnum(data);
}
DefFile::~DefFile()
{
    while (exports.size())
    {
        Export *one = exports.front();
        exports.pop_front();
        delete one;
    }
    while (imports.size())
    {
        Import *one = imports.front();
        imports.pop_front();
        delete one;
    }
}
void DefFile::Init()
{
    if (!initted)
    {
        initted = true;
        keywords["@"] = edt_at;
        keywords["."] = edt_dot;
        keywords["="] = edt_equals;
        keywords["NAME"] = edt_name;
        keywords["LIBRARY"] = edt_library;
        keywords["EXPORTS"] = edt_exports;
        keywords["IMPORTS"] = edt_imports;
        keywords["DESCRIPTION"] = edt_description;
        keywords["STACKSIZE"] = edt_stacksize;
        keywords["HEAPSIZE"] = edt_heapsize;
        keywords["CODE"] = edt_code;
        keywords["DATA"] = edt_data;
        keywords["SECTIONS"] = edt_sections;
        keywords["NONAME"] = edt_noname;
        keywords["CONSTANT"] = edt_constant;
        keywords["PRIVATE"] = edt_private;
        keywords["READ"] = edt_read;
        keywords["WRITE"] = edt_write;
        keywords["EXECUTE"] = edt_execute;
        keywords["SHARED"] = edt_shared;
    }
}
bool DefFile::Read()
{
    stream = new std::fstream(fileName.c_str(), std::ios::in);
    if (!stream->fail())
    {
        try
        {
            lineno = 0;
            NextToken();
            while (!stream->eof())
            {
                if (token->IsEnd())
                {
                    NextToken();
                }
                else if (!token->IsKeyword())
                {
                    throw new std::runtime_error("Invalid directive");
                }
                else
                {
                    switch(token->GetKeyword())
                    {
                        case edt_name:
                            ReadName();
                            break;
                        case edt_library:
                            ReadLibrary();
                            break;
                        case edt_exports:
                            ReadExports();
                            break;
                        case edt_imports:
                            ReadImports();
                            break;
                        case edt_description:
                            ReadDescription();
                            break;
                        case edt_stacksize:
                            ReadStacksize();
                            break;
                        case edt_heapsize:
                            ReadHeapsize();
                            break;
                        case edt_code:
                            ReadCode();
                            break;
                        case edt_data:
                            ReadData();
                            break;
                        case edt_sections:
                            ReadSections();
                            break;
                        default:
                            throw new std::runtime_error("Invalid directive");
                    }
                }
            }
        }
        catch (std::runtime_error *e)
        {
            std::cout << fileName.c_str() << "(" << lineno << "): " << e->what() << std::endl ;
            delete e;
        }
        delete stream;
    }
    else
    {
        std::cout << "File '" << fileName.c_str() << "' not found." << std::endl;
    }
    return true;
}
bool DefFile::Write()
{
    stream = new std::fstream(fileName.c_str(), std::ios::out);
    //if (!stream->fail())
    {
        WriteName();
        WriteLibrary();
        WriteExports();
        WriteImports();
        WriteDescription();
        WriteStacksize();
        WriteHeapsize();
        WriteCode();
        WriteData();
        WriteSections();
        delete stream;
    }
    return true;
}
void DefFile::NextToken()
{
    if (!stream->eof())
    {
        if (!token || token->IsEnd())
        {
            char buf[2048];
            lineno++;
            stream->getline(buf, sizeof(buf));
            if (!stream->eof())
            {
                if (stream->fail())
                    throw new std::runtime_error("I/O error");
                char *npos = strchr(buf, ';');
                if (npos)
                    *npos = 0;
                tokenizer.Reset(buf);
            }
        }
        token = tokenizer.Next();
    }
}
void DefFile::ReadName()
{
    std::string line = tokenizer.GetString();
    tokenizer.Reset("");
    token = nullptr;
    NextToken();
    
    int npos1 = line.find_first_not_of(SPACES);
    if (npos1 == std::string::npos)
        throw new std::runtime_error("Invalid NAME specifiers");
    int npos2 = line.find_first_of(COMMA SPACES, npos1);
    if (npos2 == std::string::npos)
    {
        name = line.substr(npos1);
    }
    else
    {
        name = line.substr(npos1, npos2);
        int npos3 = line.find_first_not_of(COMMA SPACES, npos2);
        if (npos3 != std::string::npos)
        {
            imageBase = strtoul(line.substr(npos3).c_str(), nullptr, 0);
        }
    }
}
void DefFile::ReadLibrary()
{
    std::string line = tokenizer.GetString();
    tokenizer.Reset("");
    token = nullptr;
    NextToken();
    
    int npos1 = line.find_first_not_of(SPACES);
    if (npos1 == std::string::npos)
        throw new std::runtime_error("Invalid NAME specifiers");
    int npos2 = line.find_first_of(COMMA SPACES, npos1);
    if (npos2 == std::string::npos)
    {
        library = line.substr(npos1);
    }
    else
    {
        library = line.substr(npos1, npos2);
        int npos3 = line.find_first_not_of(COMMA SPACES, npos2);
        if (npos3 != std::string::npos)
        {
            imageBase = strtoul(line.substr(npos3).c_str(), nullptr, 0);
        }
    }
}
void DefFile::ReadExports()
{
    NextToken();
    if (token->IsEnd())
    {
        while (token->IsEnd() && !stream->eof())
            NextToken();
        while (token->IsIdentifier())
        {
            Export *oneExport = new Export;
            oneExport->id = token->GetId();
            NextToken();
            if (token->GetKeyword() == edt_equals)
            {
                NextToken();
                if (!token->IsIdentifier())
                    throw new std::runtime_error("Expected entry specifier");
                oneExport->entry = token->GetId();
                NextToken();
                if (token->GetKeyword() == edt_dot)
                {
                    NextToken();
                    std::string line = tokenizer.GetString();
                    int npos = line.find_first_of(SPACES);
                    if (npos != std::string::npos)
                    {
                        oneExport->module = line.substr(0, npos);
                        tokenizer.Reset(line.substr(npos));
                    }
                    else
                    {
                        oneExport->module = line;
                        tokenizer.Reset("");
                        token = nullptr;
                    }
                    tokenizer.Next();
                }
            }
            else
            {
                if (token->IsIdentifier())
                {
                    oneExport->entry = token->GetId();
                    NextToken();
                }
                else
                {
                    oneExport->entry = oneExport->id;
                }
            }
            if (token->GetKeyword() == edt_at)
            {
                NextToken();
                if (token->IsNumeric())
                {
                    oneExport->ord = token->GetInteger();
                }
                else
                {
                    throw new std::runtime_error("Expected ordinal value");
                }
                NextToken();
            }
            bool done = false;
            while (!done && token->IsKeyword())
            {
                switch (token->GetKeyword())
                {
                    case edt_noname:
                        oneExport->byOrd = true;
                        NextToken();
                        break;
                    case edt_constant:
                    case edt_private:
                    case edt_data:
                        NextToken();
                        break;
                    default:
                        done = true;
                        break;
                }
            }
            exports.push_back(oneExport);
            if (!token->IsEnd())
                break;
            while (token->IsEnd() && !stream->eof())
                NextToken();
        }
    }
}
void DefFile::ReadImports()
{
    NextToken();
    if (token->IsEnd())
    {
        while (token->IsEnd() && !stream->eof())
            NextToken();
        while (token->IsIdentifier())
        {
            Import *oneImport = new Import;
            oneImport->id = token->GetId();
            NextToken();
            if (token->GetKeyword() == edt_dot)
            {
                oneImport->module = oneImport->id;
                NextToken();
                if (!token->IsIdentifier())
                    throw new std::runtime_error("Expected id specifier");
                oneImport->id = oneImport->entry = token->GetId();
            }
            else if (token->GetKeyword() == edt_equals)
            {
                std::string line = tokenizer.GetString();
                int npos = line.find_first_of(".");
                if (npos != std::string::npos)
                {
                    oneImport->module = line.substr(0, npos);
                    tokenizer.Reset(line.substr(npos));
                }
                else
                {
                    throw new std::runtime_error("Expected id specifier");
                }
                NextToken();
                if (!token->IsIdentifier())
                    throw new std::runtime_error("Expected id specifier");
                oneImport->id =  token->GetId();
            }
            imports.push_back(oneImport);
            if (!token->IsEnd())
                break;
            while (token->IsEnd() && !stream->eof())
                NextToken();
        }
    }
}
void DefFile::ReadDescription()
{
    description = tokenizer.GetString();
    if (!description.size())
        throw new std::runtime_error("Expected Description");
    tokenizer.Reset("");
    token = nullptr;
    NextToken();
}
void DefFile::ReadStacksize()
{
    NextToken();
    if (!token->IsNumeric())
        throw new std::runtime_error("Expected stack size");
    stackSize = token->GetInteger();
    NextToken();
}
void DefFile::ReadHeapsize()
{
    NextToken();
    if (!token->IsNumeric())
        throw new std::runtime_error("Expected heap size");
    heapSize = token->GetInteger();
    NextToken();
}
void DefFile::ReadSectionsInternal(const char *name)
{
    bool done = false;
    int flags = 0;
    while (!done && token->IsKeyword())
    {
        switch(token->GetKeyword())
        {
               case edt_read:
                 flags |= WINF_READABLE;
                break;
            case edt_write:
                flags |= WINF_WRITEABLE;
                break;
            case edt_execute:
                flags |= WINF_EXECUTE;
                break;
            case edt_shared:
                flags |= WINF_SHARED;
                break;
            default:
                done = true;
                break;
        }
    }
    sectionMap[name] = flags;
}
void DefFile::ReadCode()
{
    NextToken();
    ReadSectionsInternal("CODE");
}
void DefFile::ReadData()
{
    NextToken();
    ReadSectionsInternal("DATA");
}
void DefFile::ReadSections()
{
    NextToken();
    bool done = false;
    while (!done)
    {
        if (token->IsKeyword() || token->IsEnd())
        {
            done = true;
        }
        else
        {
            std::string line = token->GetChars() + tokenizer.GetString();
            int npos = line.find_first_of(SPACES);
            if (npos == std::string::npos)
            {
                sectionMap[line] = 0;
            }
            else
            {
                std::string name = line.substr(0, npos);
                tokenizer.Reset(line.substr(npos));
                NextToken();
                ReadSectionsInternal(name.c_str());
            }
        }
    }
}
void DefFile::WriteName()
{
    if (name.size())
    {
        *stream << "NAME " << name.c_str() ;
        if (imageBase != -1)
        {
            *stream << ", " << imageBase ;
        }
        *stream << std::endl;
    }
}
void DefFile::WriteLibrary()
{
    if (library.size())
    {
        *stream << "LIBRARY " << library.c_str() ;
        if (imageBase != -1)
        {
            *stream << ", " << imageBase ;
        }
        *stream << std::endl;
    }
}
void DefFile::WriteExports()
{
    bool first = true;
    for (auto exp : exports)
    {
        if (first)
        {
            *stream << "EXPORTS" << std::endl;
            first = false;
        }
        *stream << "\t" << exp->id.c_str();
        if ((exp->entry.size() && exp->entry != exp->id) || exp->module.size())
        {
            *stream << "=" << exp->entry.c_str();
            if (exp->module.size())
                *stream << "." << exp->module.c_str();
        }
        if (exp->ord != -1)
            *stream << " @" << exp->ord;
        if (exp->byOrd)
            *stream << " " << "NONAME";
        *stream << std::endl;
    }
}
void DefFile::WriteImports()
{
    bool first = true;
    for (auto import : imports)
    {
        if (first)
        {
            *stream << "IMPORTS" << std::endl;
            first = false;
        }
        if (import->module.size())
        {
            if (import->entry == import->id)
            {
                *stream << "\t" << import->module.c_str() << "." << import->id.c_str();
            }
            else
            {
                *stream << "\t" << import->id.c_str() << "=" << import->module.c_str() << "." << import->entry.c_str();
            }
        }
        else
        {
            *stream << "\t" << import->id.c_str();
        }
        *stream << std::endl;
    }
}
void DefFile::WriteDescription()
{
    if (description.size())
        *stream << description.c_str() << std::endl;
}
void DefFile::WriteStacksize()
{
    if (stackSize != -1)
    {
        *stream << "STACKSIZE " << stackSize << std::endl;
    }
}
void DefFile::WriteHeapsize()
{
    if (heapSize != -1)
    {
        *stream << "HEAPSIZE " << heapSize << std::endl;
    }
}
void DefFile::WriteSectionBits(unsigned value)
{
    if (value & WINF_READABLE)
        *stream << "READ ";
    if (value & WINF_WRITEABLE)
        *stream << "WRITE ";
    if (value & WINF_EXECUTE)
        *stream << "EXECUTE ";
    if (value & WINF_SHARED)
        *stream << "SHARED ";
    *stream << std::endl;
}
void DefFile::WriteCode()
{
    for (auto section : sectionMap)
    {
        if (section.first == "CODE")
        {
            *stream << "CODE " << section.second << std::endl;
            WriteSectionBits(section.second);
            break;
        }
    }
}
void DefFile::WriteData()
{
    for (auto section : sectionMap)
    {
        if (section.first == "DATA")
        {
            *stream << "DATA " ;
            WriteSectionBits(section.second);
            break;
        }
    }
}
void DefFile::WriteSections()
{
    bool first = true;
    for (auto section : sectionMap)
    {
        if (section.first != "CODE" && section.first != "DATA")
        {
            if (first)
            {
                std::cout << "SECTIONS" << std::endl;
                first = false;
            }
            *stream << "\t" << section.first.c_str() << " " ;
            WriteSectionBits(section.second);
            break;
        }
    }
}
