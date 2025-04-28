/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 *
 */

#include "AsmFile.h"
#include "Utils.h"
#include "PreProcessor.h"
#include "ObjIeee.h"
#include "ObjSection.h"
#include "ObjFactory.h"
#include "ObjSymbol.h"
#include "Instruction.h"
#include "InstructionParser.h"
#include "Section.h"
#include "Label.h"
#include "Fixup.h"
#include "Listing.h"
#include "UTF8.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <climits>
#include <algorithm>

std::vector<std::shared_ptr<Section>> AsmFile::numericSections;
std::unordered_map<ObjString, std::shared_ptr<Section>> AsmFile::labelSections;

AsmFile::~AsmFile()
{
    Section::sections.clear();
    numericSections.clear();
}
bool AsmFile::Read()
{
    bool rv = true;
    AsmExpr::ReInit();
    parser->Init();
    bigEndian = parser->IsBigEndian();
    FPF::SetBigEndian(bigEndian);
    Instruction::SetBigEndian(bigEndian);
    listing.SetBigEndian(bigEndian);
    while (!lexer.AtEof())
    {
        bool inInstruction = false;
        try
        {
            if (GetKeyword() == kw::openbr)
            {
                Directive();
                thisLabel = nullptr;
            }
            else if (GetKeyword() == kw::begin || parser->MatchesOpcode(GetToken()->GetChars()))
            {
                std::string eol = lexer.GetRestOfLine(false);
                int npos = eol.find_first_not_of(" \r\n\v\t");
                if (npos != std::string::npos && eol[npos] == ':')
                {
                    // treat as label
                    int lineno = preProcessor.GetMainLineNo();
                    std::string name = GetToken()->GetChars();
                    NextToken();
                    DoLabel(name, lineno);
                }
                else
                {
                    // assume instruction
                    NoAbsolute();
                    NeedSection();
                    inInstruction = true;
                    int lineno = preProcessor.GetMainLineNo();
                    std::shared_ptr<Instruction> ins = parser->Parse(lexer.GetRestOfLine(), currentSection->GetPC());
                    if (lineno >= 0)
                        listing.Add(ins, lineno, preProcessor.InMacro());
                    NextToken();
                    currentSection->InsertInstruction(ins);
                    thisLabel = nullptr;
                }
            }
            else
            {
                int lineno = preProcessor.GetMainLineNo();
                std::string name = GetId();
                DoLabel(name, lineno);
            }
        }
        catch (std::runtime_error* e)
        {
            Errors::Error(e->what());
            delete e;
            rv = false;
            if (inInstruction)
                NextToken();
            else
                lexer.SkipPastEol();
        }
    }
    for (const auto& exp : exports)
    {
        if (labels[exp.first] == nullptr)
        {
            Errors::Error(std::string("Undefined export symbol '") + exp.first + "'");
            rv = false;
        }
        else
        {
            labels[exp.first]->SetPublic(true);
        }
    }
    for (const auto& global : globals)
    {
        if (labels[global] == nullptr)
        {
            Errors::Error(std::string("Undefined public '") + global + "'");
            rv = false;
        }
        else
        {
            labels[global]->SetPublic(true);
        }
    }
    return rv && !Errors::GetErrorCount();
}
void AsmFile::DoLabel(std::string& name, int lineno)
{
    NeedSection();
    std::shared_ptr<Label> label;
    if (caseInsensitive)
    {
        name = UTF8::ToUpper(name);
    }
    std::string realName = name;
    bool nl = false;
    if (name.size() > 2)
    {
        if (name[0] == '.' && name[1] == '.' && name[2] == '@')
        {
            if (name.size() == 3)
                throw new std::runtime_error("Malformed non-local label");
            nl = true;
        }
    }
    if (!nl && name[0] == '.')
    {
        if (name == "..start")
        {
            if (startupSection)
            {
                throw new std::runtime_error("Multiple start addresses specified");
            }
            //            label = new Label(name, labels.size(), currentSection->GetSect());
            startupSection = currentSection;
        }
        else
        {
            if (currentLabel)
            {
                realName = currentLabel->GetName() + name;
            }
        }
    }
    if (labels.find(realName) != labels.end())
    {
        if (realName != "..start")
        {
            throw new std::runtime_error(std::string("Label '") + name + "' already exists.");
        }
    }
    else
    {
        if (inAbsolute)
        {
            labels[realName] = std::make_shared<Label>(realName, labels.size(), 0);
            label = labels[realName];
            label->SetOffset(absoluteValue);
            auto val = std::make_shared<AsmExprNode>(absoluteValue);
            AsmExpr::SetEqu(realName, val);
            if (lineno >= 0)
                listing.Add(label, lineno, preProcessor.InMacro());
        }
        else
        {
            labels[realName] = std::make_shared<Label>(realName, labels.size(), currentSection->GetSect() - 1);
            label = labels[realName];
        }
        if (name[0] != '.')
        {
            currentLabel = label;
            AsmExpr::SetCurrentLabel(label->GetName());
        }
        thisLabel = label;
        numericLabels.push_back(label);
        if (!inAbsolute)
            currentSection->InsertLabel(label);
        //		if (lineno >= 0)
        //			listing.Add(thisLabel, lineno, preProcessor.InMacro());
        if (realName == "..start")
            startupLabel = std::move(label);
    }
    if (GetKeyword() == kw::colon)
    {
        NextToken();
        thisLabel = nullptr;
    }
}
void AsmFile::DoDB()
{
    char buf[3000];
    int size = 0;
    bool byte = GetKeyword() == kw::DB;
    short val = 0;
    std::deque<std::shared_ptr<Fixup>> fixups;
    NeedSection();
    int lineno = preProcessor.GetMainLineNo();
    do
    {
        int errLine = Errors::GetErrorLine();
        std::string errFile = Errors::GetFileName();
        NextToken();
        if (IsString())
        {
            std::wstring str = GetString();
            int len = str.size();
            if (byte)
            {
                for (int i = 0; i < len; i++)
                {
                    buf[size++] = str[i];
                }
            }
            else
            {
                char temp[1000];
                int j;
                for (j = 0; j < len; j++)
                    temp[j] = str[j];
                temp[j] = 0;
                for (int i = 0; i < len;)
                {
                    int v = UTF8::Decode(temp + i);
                    if (v < 0x10000)
                    {
                        (*(short*)(buf + size)) = v;
                        size += 2;
                    }
                    else
                    {
                        v -= 0x10000;
                        int n1 = v >> 10;
                        int n2 = v & 0x3ff;
                        *((short*)(buf + size)) = n1 + 0xd800;
                        *((short*)(buf + size + 2)) = n2 + 0xdc00;
                        size += 4;
                    }
                    i += UTF8::CharSpan(temp + i);
                }
            }
        }
        else
        {
            std::shared_ptr<AsmExprNode> num = GetNumber();
            std::shared_ptr<Fixup> f = std::make_shared<Fixup>(num, byte ? 1 : 2, false, 0);
            f->SetInsOffs(size);
            f->SetFileName(errFile);
            f->SetErrorLine(errLine);
            fixups.push_back(std::move(f));
            buf[size++] = 0;
            if (!byte)
                buf[size++] = 0;
        }
    } while (GetKeyword() == kw::comma);
    std::shared_ptr<Instruction> ins = std::make_shared<Instruction>((unsigned char*)buf, size, true);
    if (lineno >= 0)
        listing.Add(ins, lineno, preProcessor.InMacro());
    currentSection->InsertInstruction(ins);
    while (fixups.size())
    {
        std::shared_ptr<Fixup> f = fixups.front();
        fixups.pop_front();
        ins->Add(std::move(f));
    }
}
void AsmFile::DoDD()
{
    char buf[3000];
    int size = 0;
    NeedSection();
    int val = 0;
    int lineno = preProcessor.GetMainLineNo();
    std::deque<std::shared_ptr<Fixup>> fixups;
    do
    {
        int errLine = Errors::GetErrorLine();
        std::string errFile = Errors::GetFileName();
        std::shared_ptr<AsmExprNode> num;
        NextToken();
        if (IsString())
        {
            std::wstring str = GetString();
            int len = str.size();
            int i, j;
            char temp[1000];
            for (j = 0; j < len; j++)
                temp[j] = str[j];
            temp[j] = 0;
            for (i = 0; i < len;)
            {
                int v = UTF8::Decode(temp + i);
                *((unsigned*)(buf + size)) = v;
                size += 4;
                i += UTF8::CharSpan(temp + i);
            }
        }
        else
        {
            num = GetNumber();
            std::shared_ptr<Fixup> f = std::make_shared<Fixup>(num, 4, false, 0);
            f->SetInsOffs(size);
            f->SetFileName(errFile);
            f->SetErrorLine(errLine);
            fixups.push_back(std::move(f));
            *((unsigned*)(buf + size)) = 0;
            size += 4;
        }
    } while (GetKeyword() == kw::comma);
    std::shared_ptr<Instruction> ins = std::make_shared<Instruction>((unsigned char*)buf, size, true);
    if (lineno >= 0)
        listing.Add(ins, lineno, preProcessor.InMacro());
    currentSection->InsertInstruction(ins);
    while (fixups.size())
    {
        std::shared_ptr<Fixup> f = fixups.front();
        fixups.pop_front();
        ins->Add(std::move(f));
    }
}
void AsmFile::DoDQ()
{
    char buf[3000];
    int size = 0;
    NeedSection();
    int val = 0;
    int lineno = preProcessor.GetMainLineNo();
    std::deque<std::shared_ptr<Fixup>> fixups;
    do
    {
        int errLine = Errors::GetErrorLine();
        std::string errFile = Errors::GetFileName();
        std::shared_ptr<AsmExprNode> num;
        NextToken();
        num = GetNumber();
        std::shared_ptr<Fixup> f = std::make_shared<Fixup>(num, 8, false, 0);
        f->SetInsOffs(size);
        f->SetFileName(errFile);
        f->SetErrorLine(errLine);
        fixups.push_back(std::move(f));
        *((unsigned long long*)(buf + size)) = 0;
        size += 8;
    } while (GetKeyword() == kw::comma);
    std::shared_ptr<Instruction> ins = std::make_shared<Instruction>((unsigned char*)buf, size, true);
    if (lineno >= 0)
        listing.Add(ins, lineno, preProcessor.InMacro());
    currentSection->InsertInstruction(ins);
    while (fixups.size())
    {
        std::shared_ptr<Fixup> f = fixups.front();
        fixups.pop_front();
        ins->Add(std::move(f));
    }
}
void AsmFile::DoFloat()
{
    unsigned char buf[4000];
    int size = 0;
    bool tbyte = GetKeyword() == kw::DT;
    NeedSection();
    int lineno = preProcessor.GetMainLineNo();
    std::deque<std::shared_ptr<Fixup>> fixups;
    memset(buf, 0, sizeof(buf));
    do
    {
        int errLine = Errors::GetErrorLine();
        std::string errFile = Errors::GetFileName();
        int lineno = preProcessor.GetMainLineNo();
        std::shared_ptr<AsmExprNode> num;
        NextToken();
        num = GetNumber();
        std::shared_ptr<Fixup> f = std::make_shared<Fixup>(num, tbyte ? 10 : 8, false, 0);
        f->SetInsOffs(size);
        f->SetFileName(errFile);
        f->SetErrorLine(errLine);
        fixups.push_back(std::move(f));
        size += tbyte ? 10 : 8;
    } while (GetKeyword() == kw::comma);
    std::shared_ptr<Instruction> ins = std::make_shared<Instruction>((unsigned char*)buf, size, true);
    if (lineno >= 0)
        listing.Add(ins, lineno, preProcessor.InMacro());
    currentSection->InsertInstruction(ins);
    while (fixups.size())
    {
        std::shared_ptr<Fixup> f = fixups.front();
        fixups.pop_front();
        ins->Add(std::move(f));
    }
}
void AsmFile::ReserveDirective(int n)
{
    NeedSection();
    NextToken();
    int errLine = Errors::GetErrorLine();
    std::string errFile = Errors::GetFileName();
    int lineno = preProcessor.GetMainLineNo();
    int num = GetValue();
    if (num <= 0)
        throw new std::runtime_error("Invalid reserve size");
    std::shared_ptr<Instruction> ins = std::make_shared<Instruction>(num, n);
    bool added = false;
    if (lineno >= 0)
    {
        listing.Add(ins, lineno, preProcessor.InMacro());
        added = true;
    }
    if (inAbsolute)
    {
        absoluteValue += n * num;
    }
    else
    {
        if (GetKeyword() == kw::comma)
        {
            NextToken();
            auto num = GetNumber();
            std::shared_ptr<Fixup> f = std::make_shared<Fixup>(num, n, false, 0);
            f->SetFileName(errFile);
            f->SetErrorLine(errLine);
            ins->Add(std::move(f));
        }
        currentSection->InsertInstruction(ins);
        added = true;
    }
    if (!added)
        ins.reset();
}
void AsmFile::EquDirective()
{
    if (!thisLabel)
        throw new std::runtime_error("Label needed");
    int lineno = preProcessor.GetMainLineNo();
    NextToken();
    std::shared_ptr<AsmExprNode> num(GetNumber());
    int n = 0;
    if (inAbsolute)
        n = absoluteValue;
    else if (currentSection)
        n = currentSection->GetPC();
    num = (AsmExpr::ConvertToBased(num, n));
    if (num->IsAbsolute())
    {
        num = (AsmExpr::Eval(num, n));
    }
    thisLabel->SetOffset(num);
    thisLabel->SetSect(-1);
    if (lineno >= 0)
        listing.Add(thisLabel, lineno, preProcessor.InMacro());
    if (currentSection && !inAbsolute)
        currentSection->pop_back();
    AsmExpr::SetEqu(thisLabel->GetName(), num);
    num = nullptr;
}
void AsmFile::Directive()
{
    NextToken();
    switch (GetKeyword())
    {
        case kw::UNKNOWNDIRECTIVE:
            UnknownDirective();
            break;
        case kw::STRING:
            NoAbsolute();
            StringDirective();
            break;
        case kw::SINGLE:
            NoAbsolute();
            SingleDirective();
            break;
        case kw::DOUBLE:
            NoAbsolute();
            DoubleDirective();
            break;
        case kw::EQV:
            EqvDirective();
            break;
        case kw::SET:
            SetDirective();
            break;
        case kw::ABORT:
            AbortDirective();
            break;
        case kw::ERROR:
            ErrorDirective();
            break;
        case kw::WARNING:
            WarningDirective();
            break;
        case kw::FAIL:
            FailDirective();
            break;
        case kw::FILL:
            NoAbsolute();
            FillDirective();
            break;
        case kw::SPACE:
            NoAbsolute();
            SpaceDirective();
            break;
        case kw::NOPS:
            NoAbsolute();
            NopsDirective();
            break;
        case kw::PUSHSECTION:
            PushsectionDirective();
            break;
        case kw::POPSECTION:
            PopsectionDirective();
            break;
        case kw::PREVIOUS:
            PreviousDirective();
            break;
        case kw::SUBSECTION:
            SubsectionDirective();
            break;
        case kw::GSECTION:
            GnuSectionDirective();
            break;
        case kw::EJECT:
            EjectDirective();
            break;
        case kw::PRINT:
            PrintDirective();
            break;
        case kw::TEXT:
            TextDirective();
            break;
        case kw::DATA:
            DataDirective();
            break;
        case kw::EQU:
            EquDirective();
            break;
        case kw::DB:
        case kw::DW:
            NoAbsolute();
            DoDB();
            break;
        case kw::DD:
            NoAbsolute();
            DoDD();
            break;
        case kw::DQ:
            NoAbsolute();
            DoDQ();
            break;
        case kw::DT:
            NoAbsolute();
            DoFloat();
            break;
        case kw::RESB:
            ReserveDirective(1);
            break;
        case kw::RESD:
            ReserveDirective(4);
            break;
        case kw::RESQ:
            ReserveDirective(8);
            break;
        case kw::REST:
            ReserveDirective(10);
            break;
        case kw::RESW:
            ReserveDirective(2);
            break;
        case kw::SECTION:
            SectionDirective();
            break;
        case kw::ABSOLUTE:
            AbsoluteDirective();
            break;
        case kw::PUBLIC:
            PublicDirective();
            break;
        case kw::EXTERN:
            ExternDirective();
            break;
        case kw::ALIGN:
            AlignDirective();
            break;
        case kw::BALIGN:
            GnuAlignDirective(false);
            break;
        case kw::P2ALIGN:
            GnuAlignDirective(true);
            break;
        case kw::GALIGN:
            GnuAlignDirective(true);
            break;
        case kw::INCBIN:
            NoAbsolute();
            IncbinDirective();
            break;
        case kw::IMPORT:
            NoAbsolute();
            ImportDirective();
            break;
        case kw::EXPORT:
            NoAbsolute();
            ExportDirective();
            break;
        case kw::TIMES:
            NoAbsolute();
            TimesDirective();  // timesdirective eats the ']'
            return;
        default:
            NeedSection();
            if (!GetParser()->ParseDirective(this, currentSection.get()))
                throw new std::runtime_error("Expected directive");
    }
    if (GetKeyword() == kw::closebr)
    {
        NextToken();
    }
    else
    {
        throw new std::runtime_error("Expected ']'");
    }
}
void AsmFile::UnknownDirective()
{
    NextToken();
    if (IsString())
    {
        std::wstring str = GetString();
        std::string dir;
        for (int i = 0; i < str.size(); i++)
            dir += str[i] & 255;
        if (!noGASdirectivewarning)
            Errors::Warning("Unimplemented directive: " + dir);
    }
    else
    {
        if (!noGASdirectivewarning)
            Errors::Warning("Unimplemented directive");
    }
}
void AsmFile::AlignDirective()
{
    NextToken();
    if (!IsNumber())
        throw new std::runtime_error("Alignment expected");
    if (inAbsolute)
    {
        int v = GetValue();
        int n = (absoluteValue % v);
        if (n)
            absoluteValue += n - v;
    }
    else
    {
        NeedSection();
        int v = GetValue();
        if ((v & (v - 1)) != 0)
            throw new std::runtime_error("Alignment must be power of two");
        std::shared_ptr<Instruction> ins = std::make_shared<Instruction>(v);
        currentSection->InsertInstruction(ins);
        int n = currentSection->GetAlign();
        if (v > n)
            currentSection->SetAlign(v);
    }
}
void AsmFile::GnuAlignDirective(bool p2)
{
    NextToken();
    if (!IsNumber())
        throw new std::runtime_error("Width expected");
    int width = GetValue();
    NextToken();  // past comma
    if (width > 4)
        width = 4;
    if (!IsNumber())
        throw new std::runtime_error("Alignment expected");
    if (inAbsolute)
    {
        int v = p2 ? 1 << GetValue() : GetValue();
        int n = (absoluteValue % v);
        if (n)
            absoluteValue += n - v;
    }
    else
    {
        NeedSection();
        int v = p2 ? 1 << GetValue() : GetValue();
        std::shared_ptr<Instruction> ins = std::make_shared<Instruction>(v);
        ins->SetFillWidth(width);
        currentSection->InsertInstruction(ins);
        int n = currentSection->GetAlign();
        if (v > n)
            currentSection->SetAlign(v);
        if (GetKeyword() == kw::comma)
        {
            NextToken();
            if (GetKeyword() != kw::comma)
            {
                if (!IsNumber())
                {
                    if (IsString())
                    {
                        std::wstring val = GetString();
                        if (val.size() != 1)
                            throw new std::runtime_error("Character constant expected");
                        ins->SetFill(val[0]);
                    }
                    else
                    {
                        throw new std::runtime_error("Fill value expected");
                    }
                }
                else
                {
                    ins->SetFill(GetValue());
                }
            }
            if (GetKeyword() == kw::comma)
            {
                NextToken();
                if (!IsNumber())
                    throw new std::runtime_error("Max value expected");
                // discarding for now, this is the maximum number of bytes to fill when doing the align
                GetValue();
            }
        }
    }
}
void AsmFile::TimesDirective()
{
    NextToken();
    NeedSection();
    int n = GetValue();
    std::string line = lexer.GetRestOfLine();
    int npos = line.find_last_of("]");
    if (npos == std::string::npos)
        throw new std::runtime_error("Expected ']'");
    if (npos != line.size() - 1)
    {
        int npos1 = line.find_first_not_of(" \t\v\n\r", npos + 1);
        if (npos1 != std::string::npos)
            throw new std::runtime_error("Extra characters on end of line");
    }
    line = line.substr(0, npos);
    lexer.StopAtEol(true);
    for (int i = 0; i < n; i++)
    {
        lexer.Reset(line);
        if (GetKeyword() == kw::openbr)
        {
            Directive();
        }
        else if (parser->MatchesOpcode(GetToken()->GetChars()))
        {
            std::shared_ptr<Instruction> ins = parser->Parse(lexer.GetRestOfLine(), currentSection->GetPC());
            for (auto& f : *ins->GetFixups())
            {
                auto expr = f->GetExpr();
                auto expr2 = AsmExpr::Eval(std::move(expr), currentSection->GetPC());
                f->SetExpr(expr2);
            }
            currentSection->InsertInstruction(ins);
        }
        else
            throw new std::runtime_error("Unknown instruction");
    }
    lexer.StopAtEol(false);
    NextToken();
}
void AsmFile::IncbinDirective()
{
    int start = 0;
    int size = INT_MAX;
    NextToken();
    NeedSection();
    if (!IsString())
        throw new std::runtime_error("File name expected");
    std::wstring wname = GetString();
    std::string name;
    for (int i = 0; i < wname.size(); i++)
    {
        char buf[2];
        buf[0] = wname[i];
        buf[1] = 0;
        name = name + buf;
    }
    if (GetKeyword() == kw::comma)
    {
        NextToken();
        start = GetValue();
        if (GetKeyword() == kw::comma)
        {
            NextToken();
            size = GetValue();
        }
    }
    std::fstream in(name.c_str(), std::ios::in | std::ios::binary);
    if (!in.is_open())
        throw new std::runtime_error(std::string("File '") + name + "' not found.");
    in.seekg(0, std::ios::end);
    int len = in.tellg();
    if (start > len)
        throw new std::runtime_error("Data not found.");
    if (size == INT_MAX)
        size = len - start;
    else if (len - start < size)
        throw new std::runtime_error("Not enough data.");
    in.seekg(start, std::ios::beg);
    std::unique_ptr<unsigned char[]> data = std::make_unique<unsigned char[]>(size);
    in.read((char*)data.get(), size);
    std::shared_ptr<Instruction> ins = std::make_shared<Instruction>(data.get(), size);
    currentSection->InsertInstruction(ins);
}
void AsmFile::PublicDirective()
{
    do
    {
        NextToken();
        std::string name = GetId();
        if (caseInsensitive)
        {
            name = UTF8::ToUpper(name);
        }
        globals.insert(name);
    } while (GetKeyword() == kw::comma);
}
void AsmFile::InsertExtern(const std::string& name1)
{
    std::string name = name1;
    if (caseInsensitive)
    {
        name = UTF8::ToUpper(name);
    }
    externs.insert(name);
    if (labels.find(name) != labels.end() && !labels[name]->IsExtern())
    {
        throw new std::runtime_error(std::string("Label '") + name + "' already exists.");
    }
    else
    {
        if (labels.find(name) == labels.end())
        {
            labels[name] = std::make_shared<Label>(name, labels.size(), Section::sections.size() - 1);
        }
        std::shared_ptr<Label> label = labels[name];
        label->SetExtern(true);
        numericLabels.push_back(std::move(label));
    }
}
void AsmFile::ResolveAttExterns(const std::set<std::string>& externs)
{
    for (auto&& name : externs)
    {
        auto it = labels.find(name);
        if (it == labels.end())
        {
            InsertExtern(name);
        }
    }
}
void AsmFile::ExternDirective()
{
    do
    {
        NextToken();
        std::string name = GetId();
        InsertExtern(name);
    } while (GetKeyword() == kw::comma);
}
void AsmFile::ImportDirective()
{
    NextToken();
    std::string internal = GetId();
    std::string dll;
    if (GetKeyword() != kw::closebr)
        dll = GetId();
    std::string external;
    if (GetKeyword() != kw::closebr)
        external = GetId();
    else
        external = internal;
    imports[internal] = std::make_unique<Import>();
    Import* imp = imports[internal].get();
    imp->dll = std::move(dll);
    imp->extname = std::move(external);
}
void AsmFile::ExportDirective()
{
    NextToken();
    std::string internal = GetId();
    std::string external;
    if (GetKeyword() != kw::closebr)
        external = GetId();
    else
        external = internal;
    exports[internal] = std::move(external);
}
void AsmFile::SectionDirective()
{
    NextToken();
    std::string name = GetId();
    if (Section::sections[name] == nullptr)
    {
        Section::sections[name] = std::make_shared<Section>(name, Section::sections.size());
        std::shared_ptr<Section> section = Section::sections[name];
        ;
        numericSections.push_back(section);
        section->Parse(this);
        currentSection = std::move(section);
    }
    else
    {
        currentSection = Section::sections[name];
    }
    AsmExpr::SetSection(currentSection);
    parser->Setup(currentSection.get());
    currentLabel = nullptr;
    AsmExpr::SetCurrentLabel(std::string(""));
    inAbsolute = false;
}
void AsmFile::AbsoluteDirective()
{
    NeedSection();
    NextToken();
    absoluteValue = GetValue();
    inAbsolute = true;
}
void AsmFile::StringDirective()
{
    char buf[3000];
    int size = 0;
    short val = 0;
    NeedSection();
    int lineno = preProcessor.GetMainLineNo();
    NextToken();
    int width = GetValue();
    do
    {
        int errLine = Errors::GetErrorLine();
        std::string errFile = Errors::GetFileName();
        NextToken();
        std::wstring str = GetString();
        int len = str.size();
        for (int i = 0; i < len; i++)
        {
            buf[size++] = str[i];
            if (width > 1)
                buf[size++] = 0;
            if (width > 2)
            {
                buf[size++] = 0;
                buf[size++] = 0;
            }
        }
    } while (GetKeyword() == kw::comma);
    std::shared_ptr<Instruction> ins = std::make_shared<Instruction>((unsigned char*)buf, size, true);
    if (lineno >= 0)
        listing.Add(ins, lineno, preProcessor.InMacro());
    currentSection->InsertInstruction(ins);
}
void AsmFile::SingleDirective()
{
    unsigned char buf[4000];
    int size = 0;
    NeedSection();
    int lineno = preProcessor.GetMainLineNo();
    std::deque<std::shared_ptr<Fixup>> fixups;
    memset(buf, 0, sizeof(buf));
    do
    {
        int errLine = Errors::GetErrorLine();
        std::string errFile = Errors::GetFileName();
        int lineno = preProcessor.GetMainLineNo();
        std::shared_ptr<AsmExprNode> num;
        NextToken();
        num = GetNumber();
        if (num->GetType() == AsmExprNode::IVAL)
        {
            num->fval = num->ival;
            num->SetType(AsmExprNode::FVAL);
        }

        std::shared_ptr<Fixup> f = std::make_shared<Fixup>(num, 4, false, 0);
        f->SetInsOffs(size);
        f->SetFileName(errFile);
        f->SetErrorLine(errLine);
        fixups.push_back(std::move(f));
        size += 4;
    } while (GetKeyword() == kw::comma);
    std::shared_ptr<Instruction> ins = std::make_shared<Instruction>((unsigned char*)buf, size, true);
    if (lineno >= 0)
        listing.Add(ins, lineno, preProcessor.InMacro());
    currentSection->InsertInstruction(ins);
    while (fixups.size())
    {
        std::shared_ptr<Fixup> f = fixups.front();
        fixups.pop_front();
        ins->Add(std::move(f));
    }
}
void AsmFile::DoubleDirective()
{
    unsigned char buf[4000];
    int size = 0;
    NeedSection();
    int lineno = preProcessor.GetMainLineNo();
    std::deque<std::shared_ptr<Fixup>> fixups;
    memset(buf, 0, sizeof(buf));
    do
    {
        int errLine = Errors::GetErrorLine();
        std::string errFile = Errors::GetFileName();
        int lineno = preProcessor.GetMainLineNo();
        std::shared_ptr<AsmExprNode> num;
        NextToken();
        num = GetNumber();
        if (num->GetType() == AsmExprNode::IVAL)
        {
            num->fval = num->ival;
            num->SetType(AsmExprNode::FVAL);
        }
        std::shared_ptr<Fixup> f = std::make_shared<Fixup>(num, 8, false, 0);
        f->SetInsOffs(size);
        f->SetFileName(errFile);
        f->SetErrorLine(errLine);
        fixups.push_back(std::move(f));
        size += 8;
    } while (GetKeyword() == kw::comma);
    std::shared_ptr<Instruction> ins = std::make_shared<Instruction>((unsigned char*)buf, size, true);
    if (lineno >= 0)
        listing.Add(ins, lineno, preProcessor.InMacro());
    currentSection->InsertInstruction(ins);
    while (fixups.size())
    {
        std::shared_ptr<Fixup> f = fixups.front();
        fixups.pop_front();
        ins->Add(std::move(f));
    }
}
void AsmFile::EqvDirective()
{
    // Eqv shouldn't evaluate immediately, but this will do for now...
    SetDirective();
}
void AsmFile::SetDirective()
{
    NextToken();
    int lineno = preProcessor.GetMainLineNo();
    std::string name = GetId();
    DoLabel(name, lineno);
    EquDirective();
}
void AsmFile::AbortDirective() { exit(1); }
void AsmFile::ErrorDirective()
{
    NextToken();
    if (IsString())
    {
        std::wstring str = GetString();
        int len = str.size();
        std::string err;
        for (int i = 0; i < str.size(); i++)
            err += (char)str[i];
        Errors::Error(err);
    }
    else
    {
        Errors::Error(".err directive");
    }
}
void AsmFile::WarningDirective()
{
    NextToken();
    if (IsString())
    {
        std::wstring str = GetString();
        int len = str.size();
        std::string err;
        for (int i = 0; i < str.size(); i++)
            err += (char)str[i];
        Errors::Warning(err);
    }
    else
    {
        Errors::Warning("");
    }
}
void AsmFile::FailDirective()
{
    NextToken();
    if (!IsNumber())
        throw new std::runtime_error("Expected fail value");
    int n = GetValue();
    char buf[256];
    sprintf(buf, "%d", n);
    if (n >= 500)
        Errors::Warning(std::string("fail code ") + buf);
    else
        Errors::Error(std::string("fail code ") + buf);
}
void AsmFile::FillDirective()
{
    NextToken();
    if (!IsNumber())
        throw new std::runtime_error("Repeat expected");
    int lineno = preProcessor.GetMainLineNo();
    NeedSection();
    int repeat = GetValue();
    int size = 1;
    int value = 0;
    if (GetKeyword() == kw::comma)
    {
        NextToken();
        if (GetKeyword() != kw::comma)
        {
            if (!IsNumber())
                throw new std::runtime_error("Size value expected");
            size = GetValue();
            if (size > 8)
                size = 8;
        }
        if (GetKeyword() == kw::comma)
        {
            NextToken();
            if (!IsNumber())
            {
                if (IsString())
                {
                    std::wstring val = GetString();
                    if (val.size() != 1)
                        throw new std::runtime_error("Character constant expected");
                    value = val[0];
                }
                else
                {
                    throw new std::runtime_error("Fill value expected");
                }
            }
            else
            {
                value = GetValue() & 0xffffffff;
            }
        }
    }
    if (repeat && size)
    {
        unsigned char val[8];
        memset(val, 0, sizeof(val));
        *(unsigned*)val = value;
        unsigned char* buf = new unsigned char[repeat * size];
        for (int i = 0; i < repeat; i++)
        {
            memcpy(buf + i * size, val, size);
        }
        std::shared_ptr<Instruction> ins = std::make_shared<Instruction>((unsigned char*)buf, repeat * size, true);
        if (lineno >= 0)
            listing.Add(ins, lineno, preProcessor.InMacro());
        currentSection->InsertInstruction(ins);
    }
}
void AsmFile::SpaceDirective()
{
    NextToken();
    if (!IsNumber())
        throw new std::runtime_error("Repeat expected");
    int lineno = preProcessor.GetMainLineNo();
    NeedSection();
    int repeat = GetValue();
    int value = 0;
    if (GetKeyword() == kw::comma)
    {
        NextToken();
        if (!IsNumber())
        {
            if (IsString())
            {
                std::wstring val = GetString();
                if (val.size() != 1)
                    throw new std::runtime_error("Character constant expected");
                value = val[0];
            }
            else
            {
                throw new std::runtime_error("Fill value expected");
            }
        }
        else
        {
            value = GetValue() & 0xffffffff;
        }
    }
    if (repeat)
    {
        unsigned char* buf = new unsigned char[repeat];
        memset(buf, value, repeat);
        std::shared_ptr<Instruction> ins = std::make_shared<Instruction>(buf, repeat, true);
        if (lineno >= 0)
            listing.Add(ins, lineno, preProcessor.InMacro());
        currentSection->InsertInstruction(ins);
    }
}
void AsmFile::NopsDirective()
{
    NextToken();
    if (!IsNumber())
        throw new std::runtime_error("Repeat expected");
    int lineno = preProcessor.GetMainLineNo();
    NeedSection();
    int repeat = GetValue();
    int value = 0x90;  // processor specific
    if (GetKeyword() == kw::comma)
    {
        NextToken();
        if (!IsNumber())
            throw new std::runtime_error("control value expected");
        // we are being naive and just nop filling
        GetValue();
    }
    if (repeat)
    {
        unsigned char* buf = new unsigned char[repeat];
        memset(buf, value, repeat);
        std::shared_ptr<Instruction> ins = std::make_shared<Instruction>(buf, repeat, true);
        if (lineno >= 0)
            listing.Add(ins, lineno, preProcessor.InMacro());
        currentSection->InsertInstruction(ins);
    }
}
void AsmFile::GnuSectionDirective()
{
    NextToken();
    std::string name = GetId();
    if (Section::sections[name] == nullptr)
    {
        Section::sections[name] = std::make_shared<Section>(name, Section::sections.size());
        std::shared_ptr<Section> section = Section::sections[name];
        ;
        numericSections.push_back(section);
        section->Parse(this);
        currentSection = std::move(section);
    }
    else
    {
        currentSection = Section::sections[name];
    }
    if (GetKeyword() == kw::comma)
    {
        NextToken();
        if (IsString())
            Errors::Warning(".section: ignoring flags");
        else if (IsNumber())
            SetSubsection(currentSection, GetValue());
        else
            throw new std::runtime_error("Expected subsection id");
    }
    else
    {
        SetSubsection(currentSection, 0);
    }
    AsmExpr::SetSection(currentSection);
    parser->Setup(currentSection.get());
    currentLabel = nullptr;
    AsmExpr::SetCurrentLabel(std::string(""));
    inAbsolute = false;
}
void AsmFile::SubsectionDirective()
{
    NextToken();
    if (!IsNumber())
        throw new std::runtime_error("Expected subsection id");
    SetSubsection(currentSection, GetValue());
}
void AsmFile::PushsectionDirective()
{
    NextToken();
    if (!IsIdentifier())
        throw new std::runtime_error("Expected section name");
    std::string name = GetId();
    int subsection = 0;
    if (GetKeyword() == kw::comma)
    {
        NextToken();
        if (!IsNumber())
            throw new std::runtime_error("Expected subsection id");
        subsection = GetValue();
        while (GetKeyword() == kw::comma)
        {
            NextToken();
            NextToken();
        }
    }
    PushSection(currentSection->GetName(), currentSection->GetSubsection());
    if (Section::sections[name] == nullptr)
    {
        Section::sections[name] = std::make_shared<Section>(name, Section::sections.size());
        std::shared_ptr<Section> section = Section::sections[name];
        ;
        numericSections.push_back(section);
        section->Parse(this);
        currentSection = std::move(section);
    }
    else
    {
        currentSection = Section::sections[name];
    }
    AsmExpr::SetSection(currentSection);
    currentSection->SetSubsection(subsection);
}
void AsmFile::PopsectionDirective()
{
    NextToken();
    PopSection();
}
void AsmFile::PreviousDirective()
{
    NextToken();
    SwapSections();
}
void AsmFile::TextDirective()
{
    NextToken();
    int subsection = 0;
    if (IsNumber())
        subsection = GetValue();
    std::string name = "code";
    if (Section::sections[name] == nullptr)
    {
        Section::sections[name] = std::make_shared<Section>(name, Section::sections.size());
        std::shared_ptr<Section> section = Section::sections[name];
        ;
        numericSections.push_back(section);
        section->Parse(this);
        currentSection = std::move(section);
    }
    else
    {
        currentSection = Section::sections[name];
    }
    AsmExpr::SetSection(currentSection);
    SetSubsection(currentSection, subsection);
}
void AsmFile::DataDirective()
{
    NextToken();
    int subsection = 0;
    if (IsNumber())
        subsection = GetValue();
    std::string name = "data";
    if (Section::sections[name] == nullptr)
    {
        Section::sections[name] = std::make_shared<Section>(name, Section::sections.size());
        std::shared_ptr<Section> section = Section::sections[name];
        ;
        numericSections.push_back(section);
        section->Parse(this);
        currentSection = std::move(section);
    }
    else
    {
        currentSection = Section::sections[name];
    }
    AsmExpr::SetSection(currentSection);
    SetSubsection(currentSection, subsection);
}
void AsmFile::EjectDirective()
{
    // ignoring for now (needs a form feed in listing file)
}
void AsmFile::PrintDirective()
{
    NextToken();
    if (!IsString())
        throw new std::runtime_error("String expected");
    std::wstring str = GetString();

    std::string p;
    for (int i = 0; i < str.size(); i++)
        p += str[i] & 255;
    std::cout << p << std::endl;
}
void AsmFile::NoAbsolute()
{
    if (inAbsolute)
    {
        throw new std::runtime_error("Code generation in absolute section not allowed");
    }
}
void AsmFile::NeedSection()
{
    if (Section::sections.size() == 0 && !inAbsolute)
    {
        Section::sections["text"] = std::make_shared<Section>("text", 1);
        auto section = Section::sections["text"];
        numericSections.push_back(section);
        currentSection = std::move(section);
        AsmExpr::SetSection(currentSection);
        parser->Setup(currentSection.get());
        currentLabel = nullptr;
        AsmExpr::SetCurrentLabel(std::string(""));
    }
}
bool AsmFile::Write(std::string& fileName, std::string& srcName)
{
    bool rv = true;
    ObjIeeeIndexManager im;
    ObjFactory f(&im);
    ObjFile* fi = MakeFile(f, srcName);
    if (fi)
    {
        fi->ResolveSymbols(&f);

        //        std::fstream out(fileName.c_str(), std::fstream::trunc | std::fstream::out);
        FILE* out = fopen(fileName.c_str(), "wb");
        if (out != nullptr)
        {
            ObjIeee i(fileName.c_str());
            if (binaryOutput)
            {
                i.BinaryWrite(out, fi, &f);
            }
            else
            {

                i.SetTranslatorName(ObjString("oasm"));
                i.SetDebugInfoFlag(false);

                if (startSection)
                {
                    ObjExpression* left = f.MakeExpression(startSection);
                    ObjExpression* right = f.MakeExpression(startupLabel->GetOffset()->ival);
                    ObjExpression* sa = f.MakeExpression(ObjExpression::eAdd, left, right);
                    i.SetStartAddress(fi, sa);
                }
                i.Write(out, fi, &f);
            }
            fclose(out);
            //            out.close();
        }
        else
        {
            Utils::Fatal(std::string(std::string("Could not open ") + fileName.c_str() + " for write.").c_str());
        }
    }
    else
    {
        rv = false;
    }
    return rv;
}
ObjFile* AsmFile::MakeFile(ObjFactory& factory, std::string& name)
{
    bool rv = true;
    ObjFile* fi = factory.MakeFile(name);
    if (fi)
    {
        ResolveAttExterns(InstructionParser::GetAttExterns());

        std::time_t x = std::time(0);
        fi->SetFileTime(*std::localtime(&x));
        ObjSourceFile* sf = factory.MakeSourceFile(ObjString(name.c_str()));
        fi->Add(sf);

        labelSections.clear();
        for (int i = 0; i < numericSections.size(); ++i)
        {
            for (auto&& g : numericSections[i]->GetLabels())
            {
                labelSections[g.first] = numericSections[i];
            }
        }
        for (int i = 0; i < numericSections.size(); ++i)
        {
            numericSections[i]->MergeSections();
            numericSections[i]->Resolve(numericSections[i]);
            ObjSection* s = numericSections[i]->CreateObject(factory);
            if (s)
            {
                if (numericSections[i] == startupSection)
                {
                    startSection = s;
                }
                objSections.push_back(s);
                fi->Add(s);
            }
        }

        if (objSections.size())
        {
            for (int i = 0; i < numericLabels.size(); ++i)
            {
                std::shared_ptr<Label> l = numericLabels[i];
                if (l->GetSect() != 0xffffffff)
                {
                    l->SetObjectSection(objSections[l->GetSect()]);
                    if (l->IsPublic())
                    {
                        ObjSymbol* s1 = factory.MakePublicSymbol(l->GetName());
                        ObjExpression* left = factory.MakeExpression(objSections[l->GetSect()]);
                        ObjExpression* right = factory.MakeExpression(l->GetOffset()->ival);
                        ObjExpression* sum = factory.MakeExpression(ObjExpression::eAdd, left, right);
                        s1->SetOffset(sum);
                        fi->Add(s1);
                        l->SetObjSymbol(s1);
                    }
                    else if (l->IsExtern())
                    {
                        ObjSymbol* s1 = factory.MakeExternalSymbol(l->GetName());
                        fi->Add(s1);
                        l->SetObjSymbol(s1);
                    }
                }
                else
                {
                    ObjSymbol* s1 = factory.MakeExternalSymbol(l->GetName());
                    fi->Add(s1);
                    l->SetObjSymbol(s1);
                }
            }
        }
        for (const auto& exp : exports)
        {
            ObjExportSymbol* p = factory.MakeExportSymbol(exp.first);
            p->SetExternalName(exp.second);
            fi->Add(p);
        }
        for (auto& import : imports)
        {
            ObjImportSymbol* p = factory.MakeImportSymbol(import.first);
            p->SetExternalName(import.second->extname);
            p->SetDllName(import.second->dll);
            fi->Add(p);
        }

        for (int i = 0; i < numericSections.size(); i++)
        {
            if (!numericSections[i]->MakeData(
                    factory, [this](std::string& aa) { return Lookup(aa); },
                    [this](std::string& aa) { return GetSectionByName(aa); },
                    [](ObjFactory&, Section*, std::shared_ptr<Instruction>&) {}))
                rv = false;
        }
    }
    if (!rv)
    {
        fi = nullptr;
    }
    return fi;
}
bool AsmFile::IsKeyword()
{
    bool rv = GetToken() && GetToken()->IsKeyword();
    return rv;
}
kw AsmFile::GetKeyword() { return GetToken()->GetKeyword(); }
kw AsmFile::GetTokenId()
{
    kw rv = (kw)0;
    if (IsKeyword())
    {
        rv = GetToken()->GetKeyword();
        NextToken();
    }
    return rv;
}
bool AsmFile::IsNumber()
{
    bool rv = GetToken() &&
              (GetToken()->IsNumeric() ||
               (GetToken()->IsKeyword() && (GetToken()->GetKeyword() == kw::openpa || GetToken()->GetKeyword() == kw::plus ||
                                            GetToken()->GetKeyword() == kw::minus || GetToken()->GetKeyword() == kw::lnot ||
                                            GetToken()->GetKeyword() == kw::bcompl)));
    return rv;
}
unsigned AsmFile::GetValue()
{
    std::shared_ptr<AsmExprNode> num(GetNumber());
    if (!inAbsolute && !num->IsAbsolute())
        throw new std::runtime_error("Constant value expected");
    int n = 0;
    if (inAbsolute)
        n = absoluteValue;
    else if (currentSection)
        n = currentSection->GetPC();
    auto temp = AsmExpr::Eval(std::move(num), n);
    std::shared_ptr<AsmExprNode> num1(std::move(temp));
    if (num1->GetType() != AsmExprNode::IVAL)
        throw new std::runtime_error("Integer constant expected");
    return num1->ival;
}
std::shared_ptr<AsmExprNode> AsmFile::GetNumber()
{
    std::string line = lexer.GetRestOfLine();
    if (currentLabel)
        asmexpr.SetCurrentLabel(currentLabel->GetName());
    std::shared_ptr<AsmExprNode> rv = asmexpr.Build(line);
    if (line == "")
        NextToken();
    else
        lexer.Reset(line);
    return rv;
}
bool AsmFile::IsString() { return GetToken() && GetToken()->IsString(); }
std::wstring AsmFile::GetString()
{
    std::wstring rv;
    if (IsString())
    {
        rv = GetToken()->GetString();
        NextToken();
    }
    else
    {
        throw new std::runtime_error("String expected");
    }
    return rv;
}
bool AsmFile::IsIdentifier() { return GetToken() && GetToken()->IsIdentifier(); }
std::string AsmFile::GetId()
{
    std::string rv;
    if (IsIdentifier())
    {
        rv = GetToken()->GetId();
        if (rv[0] == '%')
            rv.erase(0, 1);
        NextToken();
    }
    else if (IsKeyword())
    {
        rv = GetToken()->GetChars();
        NextToken();
    }
    else
    {
        NextToken();
        throw new std::runtime_error("identifier expected");
    }
    return rv;
}
void AsmFile::NeedEol()
{
    if (!lexer.AtEol())
        throw new std::runtime_error("End of line expected");
}
ObjSection* AsmFile::GetSectionByName(std::string& name)
{
    auto it = Section::sections.find(name);
    if (it != Section::sections.end())
        return it->second->GetObjectSection();
    return nullptr;
}
void AsmFile::SetSubsection(std::shared_ptr<Section>& sect, int sid)
{
    sid &= 0x1fff;
    sect->SetSubsection(sid);
}
void AsmFile::PushSection(const std::string& name, int sid)
{
    auto it = Section::sections.find(name);
    if (it == Section::sections.end())
        throw new std::runtime_error("Unknown section: " + name);
    SectionPair v{it->second, sid};
    sectionStack.push(v);
}
void AsmFile::PopSection()
{
    if (sectionStack.size() != 0)
    {
        currentSection = sectionStack.top().section;
        AsmExpr::SetSection(currentSection);
        SetSubsection(currentSection, sectionStack.top().subsection);
        sectionStack.pop();
    }
}
void AsmFile::SwapSections()
{
    if (sectionStack.size() != 0)
    {
        SectionPair hold = sectionStack.top();
        sectionStack.pop();
        sectionStack.push(SectionPair{currentSection, currentSection->GetSubsection()});
        currentSection = hold.section;
        AsmExpr::SetSection(currentSection);
        SetSubsection(currentSection, hold.subsection);
    }
}
