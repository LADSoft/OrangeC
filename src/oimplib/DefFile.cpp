/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#include "ppExpr.h"
#include "DefFile.h"
#include "PEHeader.h"
#include "MZHeader.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "UTF8.h"

bool DefFile::initted;
KeywordHash DefFile::keywords;

#define SPACES " \t\v\f\r\n"
#define COMMA ","
static bool IsSymbolStartChar(const char* data)
{
    return *data == '_' || *data == '$' || *data == '?' || *data == '.' || UTF8::IsAlpha(data);
}
static bool IsSymbolChar(const char* data)
{
    return *data == '_' || *data == '$' || *data == '#' || *data == '@' || *data == '~' || *data == '?' || *data == '.' ||
           UTF8::IsAlnum(data);
}
static bool IsSymbolChar(const char* data, bool start) { return start ? IsSymbolStartChar(data) : IsSymbolChar(data); }
DefFile::~DefFile() {}
void DefFile::Init()
{
    if (!initted)
    {
        initted = true;
        keywords["@"] = kw::at;
        keywords["."] = kw::dot;
        keywords["="] = kw::equals;
        keywords["NAME"] = kw::name;
        keywords["LIBRARY"] = kw::library;
        keywords["EXPORTS"] = kw::exports;
        keywords["IMPORTS"] = kw::imports;
        keywords["DESCRIPTION"] = kw::description;
        keywords["STACKSIZE"] = kw::stacksize;
        keywords["HEAPSIZE"] = kw::heapsize;
        keywords["CODE"] = kw::code;
        keywords["DATA"] = kw::data;
        keywords["SECTIONS"] = kw::sections;
        keywords["NONAME"] = kw::noname;
        keywords["CONSTANT"] = kw::constant;
        keywords["PRIVATE"] = kw::private_;
        keywords["READ"] = kw::read;
        keywords["WRITE"] = kw::write;
        keywords["EXECUTE"] = kw::execute;
        keywords["SHARED"] = kw::shared;
        Tokenizer::IsSymbolChar = IsSymbolChar;
    }
}

void DefFile::Add(Export* e)
{
    std::unique_ptr<Export> temp(e);
    exports.push_back(std::move(temp));
}
void DefFile::Add(Import* i)
{
    std::unique_ptr<Import> temp(i);
    imports.push_back(std::move(temp));
}

bool DefFile::Read()
{
    stream.open(fileName, std::ios::in);
    if (stream.is_open())
    {
        try
        {
            lineno = 0;
            NextToken();
            while (!stream.eof())
            {
                if (token->IsEnd())
                {
                    NextToken();
                }
                else if (!token->IsKeyword())
                {
                    throw std::runtime_error("Invalid directive");
                }
                else
                {
                    switch (token->GetKeyword())
                    {
                        case kw::name:
                            ReadName();
                            break;
                        case kw::library:
                            ReadLibrary();
                            break;
                        case kw::exports:
                            ReadExports();
                            break;
                        case kw::imports:
                            ReadImports();
                            break;
                        case kw::description:
                            ReadDescription();
                            break;
                        case kw::stacksize:
                            ReadStacksize();
                            break;
                        case kw::heapsize:
                            ReadHeapsize();
                            break;
                        case kw::code:
                            ReadCode();
                            break;
                        case kw::data:
                            ReadData();
                            break;
                        case kw::sections:
                            ReadSections();
                            break;
                        default:
                            throw std::runtime_error("Invalid directive");
                    }
                }
            }
        }
        catch (std::runtime_error e)
        {
            std::cout << fileName << "(" << lineno << "): " << e.what() << std::endl;
        }
        stream.close();
    }
    else
    {
        std::cout << "File '" << fileName << "' not found." << std::endl;
    }
    return true;
}
bool DefFile::Write()
{
    stream.open(fileName, std::ios::out);
    if (stream.is_open())
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
        stream.close();
    }
    return true;
}
void DefFile::NextToken()
{
    if (!stream.eof())
    {
        if (!token || token->IsEnd())
        {
            char buf[2048];
            lineno++;
            stream.getline(buf, sizeof(buf));
            if (!stream.eof())
            {
                if (stream.fail())
                    throw std::runtime_error("I/O error");
                char* npos = (char*)strchr(buf, ';');
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
        throw std::runtime_error("Invalid NAME specifiers");
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
            imageBase = stoul(line.substr(npos3), nullptr, 0);
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
        throw std::runtime_error("Invalid NAME specifiers");
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
            imageBase = stoul(line.substr(npos3), nullptr, 0);
        }
    }
}
void DefFile::ReadExports()
{
    NextToken();
    if (token->IsEnd())
    {
        while (token->IsEnd() && !stream.eof())
            NextToken();
        while (token->IsIdentifier())
        {
            std::unique_ptr<Export> oneExport = std::make_unique<Export>();
            oneExport->id = token->GetId();
            NextToken();
            if (token->GetKeyword() == kw::equals)
            {
                NextToken();
                if (!token->IsIdentifier())
                    throw std::runtime_error("Expected entry specifier");
                oneExport->entry = token->GetId();
                NextToken();
                if (token->GetKeyword() == kw::dot)
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
                    }
                    tokenizer.Next();
                }
            }
            else
            {
                oneExport->entry = oneExport->id;
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
            if (token->GetKeyword() == kw::at)
            {
                NextToken();
                if (token->IsNumeric())
                {
                    oneExport->ord = token->GetInteger();
                }
                else
                {
                    throw std::runtime_error("Expected ordinal value");
                }
                NextToken();
            }
            bool done = false;
            while (!done && token->IsKeyword())
            {
                switch (token->GetKeyword())
                {
                    case kw::noname:
                        oneExport->byOrd = true;
                        NextToken();
                        break;
                    case kw::constant:
                    case kw::private_:
                    case kw::data:
                        NextToken();
                        break;
                    default:
                        done = true;
                        break;
                }
            }
            exports.push_back(std::move(oneExport));
            if (!token->IsEnd())
                break;
            while (token->IsEnd() && !stream.eof())
                NextToken();
        }
    }
}
void DefFile::ReadImports()
{
    NextToken();
    if (token->IsEnd())
    {
        while (token->IsEnd() && !stream.eof())
            NextToken();
        while (token->IsIdentifier())
        {
            std::unique_ptr<Import> oneImport = std::make_unique<Import>();
            oneImport->id = token->GetId();
            NextToken();
            if (token->GetKeyword() == kw::dot)
            {
                oneImport->module = oneImport->id;
                NextToken();
                if (!token->IsIdentifier())
                    throw std::runtime_error("Expected id specifier");
                oneImport->id = oneImport->entry = token->GetId();
            }
            else if (token->GetKeyword() == kw::equals)
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
                    throw std::runtime_error("Expected id specifier");
                }
                NextToken();
                if (!token->IsIdentifier())
                    throw std::runtime_error("Expected id specifier");
                oneImport->id = token->GetId();
            }
            imports.push_back(std::move(oneImport));
            if (!token->IsEnd())
                break;
            while (token->IsEnd() && !stream.eof())
                NextToken();
        }
    }
}
void DefFile::ReadDescription()
{
    description = tokenizer.GetString();
    if (description.empty())
        throw std::runtime_error("Expected Description");
    tokenizer.Reset("");
    token = nullptr;
    NextToken();
}
void DefFile::ReadStacksize()
{
    NextToken();
    if (!token->IsNumeric())
        throw std::runtime_error("Expected stack size");
    stackSize = token->GetInteger();
    NextToken();
}
void DefFile::ReadHeapsize()
{
    NextToken();
    if (!token->IsNumeric())
        throw std::runtime_error("Expected heap size");
    heapSize = token->GetInteger();
    NextToken();
}
void DefFile::ReadSectionsInternal(const char* name)
{
    bool done = false;
    int flags = 0;
    while (!done && token->IsKeyword())
    {
        switch (token->GetKeyword())
        {
            case kw::read:
                flags |= WINF_READABLE;
                break;
            case kw::write:
                flags |= WINF_WRITEABLE;
                break;
            case kw::execute:
                flags |= WINF_EXECUTE;
                break;
            case kw::shared:
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
    if (!name.empty())
    {
        stream << "NAME " << name;
        if (imageBase != -1)
        {
            stream << ", " << imageBase;
        }
        stream << std::endl;
    }
}
void DefFile::WriteLibrary()
{
    if (!library.empty())
    {
        stream << "LIBRARY " << library;
        if (imageBase != -1)
        {
            stream << ", " << imageBase;
        }
        stream << std::endl;
    }
}
void DefFile::WriteExportLine(const char *modifiedId, Export* exp)
{
    if (modifiedId)
        stream << "\t" << modifiedId;
    stream << "\t" << exp->id;
    if ((!exp->entry.empty() && exp->entry != exp->id) || !exp->module.empty())
    {
        stream << "=" << exp->entry;
        if (!exp->module.empty())
            stream << "." << exp->module;
    }
    if (!modifiedId && exp->ord != -1)
        stream << " @" << exp->ord;    
    if (exp->byOrd)
        stream << " "
               << "NONAME";
    stream << std::endl;
}
void DefFile::WriteExports()
{
    bool first = true;
    for (auto&& exp : exports)
    {
        if (first)
        {
            stream << "EXPORTS" << std::endl;
            first = false;
        }
        WriteExportLine(nullptr, exp.get());
        if (!exp->byOrd)
        {
            auto id = "_" + exp->id;
            WriteExportLine(id.c_str(), exp.get());
            int n = exp->id.find('@');
            if (n != 0 && n != std::string::npos)
            {
                auto id = exp->id.substr(0, n);
                WriteExportLine(id.c_str(), exp.get());
                if (id[0] == '_')
                {
                    id = id.substr(1, id.size()-1);
                    WriteExportLine(id.c_str(), exp.get());
                }
            }
        }
    }
}
void DefFile::WriteImports()
{
    bool first = true;
    for (auto& import : imports)
    {
        if (first)
        {
            stream << "IMPORTS" << std::endl;
            first = false;	
        }
        if (!import->module.empty())
        {
            if (import->entry == import->id)
            {
                stream << "\t" << import->module << "." << import->id;
            }
            else
            {
                stream << "\t" << import->id << "=" << import->module << "." << import->entry;
            }
        }
        else
        {
            stream << "\t" << import->id;
        }
        stream << std::endl;
    }
}
void DefFile::WriteDescription()
{
    if (!description.empty())
        stream << description << std::endl;
}
void DefFile::WriteStacksize()
{
    if (stackSize != -1)
    {
        stream << "STACKSIZE " << stackSize << std::endl;
    }
}
void DefFile::WriteHeapsize()
{
    if (heapSize != -1)
    {
        stream << "HEAPSIZE " << heapSize << std::endl;
    }
}
void DefFile::WriteSectionBits(unsigned value)
{
    if (value & WINF_READABLE)
        stream << "READ ";
    if (value & WINF_WRITEABLE)
        stream << "WRITE ";
    if (value & WINF_EXECUTE)
        stream << "EXECUTE ";
    if (value & WINF_SHARED)
        stream << "SHARED ";
    stream << std::endl;
}
void DefFile::WriteCode()
{
    for (auto section : sectionMap)
    {
        if (section.first == "CODE")
        {
            stream << "CODE " << section.second << std::endl;
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
            stream << "DATA ";
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
            stream << "\t" << section.first << " ";
            WriteSectionBits(section.second);
            break;
        }
    }
}
