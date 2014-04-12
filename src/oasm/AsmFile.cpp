// fixme endianness
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
#include <time.h>
#include <fstream>
#include <iostream>

AsmFile::~AsmFile()
{
    delete parser;
    for (int i = 0; i < numericSections.size(); i++)
    {
        Section *s = numericSections[i];
        delete s;
    }
    for (int i=0; i < numericLabels.size(); i++)
    {
        Label *l = numericLabels[i];
        delete l;
    }
    for (std::map<std::string, Import *>::iterator it = imports.begin(); it != imports.end(); ++it)
    {
        Import *i = it->second;
        delete i;
    }
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
            if (GetKeyword() == Lexer::openbr)
            {
                Directive();
                thisLabel = NULL;
            }
            else if (parser->MatchesOpcode(GetToken()->GetChars()))
            {
                NoAbsolute();
                NeedSection();
                inInstruction = true;
                int lineno = preProcessor.GetMainLineNo();
                Instruction *ins = parser->Parse(lexer.GetRestOfLine(), currentSection->GetPC());
                if (lineno >= 0)
                    listing.Add(ins, lineno, preProcessor.InMacro());
                NextToken();
                currentSection->InsertInstruction(ins);
                thisLabel = NULL;
            }
            else
            {
                int lineno = preProcessor.GetMainLineNo();
                std::string name = GetId();
                DoLabel(name, lineno);
            }
        }
        catch(std::runtime_error *e)
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
    for (std::map<std::string, std::string>::iterator it = exports.begin(); it != exports.end(); ++it)
    {
        if (labels[it->first] == NULL)
        {
            Errors::Error(std::string("Undefined export symbol '") + it->first + "'");
            rv = false;
        }
        else
        {
            labels[it->first]->SetPublic(true);
        }
    }
    for (std::set<std::string>::iterator it = globals.begin(); it != globals.end(); ++it)
    {
        if (labels[*it] == NULL)
        {
            Errors::Error(std::string("Undefined public '") + *it + "'");
            rv = false;
        }
        else
        {
            labels[*it]->SetPublic(true);
        }
    }
    return rv && !Errors::ErrorCount();
}
void AsmFile::DoLabel(std::string &name, int lineno)
{
    NeedSection();
    Label *label;
    if (caseInsensitive)
    {
        for (int i=0; i < name.size(); i++)
            name[i] = toupper(name[i]);
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
            label = new Label(name, labels.size(), currentSection->GetSect());
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
    if (labels[realName] != NULL)
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
            label = new Label(realName, labels.size(), 0);
            label->SetOffset(absoluteValue);
            AsmExpr::SetEqu(realName, new AsmExprNode(absoluteValue));
            if (lineno >= 0)
                listing.Add(label, lineno, preProcessor.InMacro());
        }
        else
        {
            label = new Label(realName, labels.size(), currentSection->GetSect()-1);
        }
        if (name[0] != '.')
        {
            currentLabel = label;
            AsmExpr::SetCurrentLabel(label->GetName());
        }
        thisLabel = label;
        labels[realName] = label;
        numericLabels.push_back(label);
        if (!inAbsolute)
            currentSection->InsertLabel(label);
//		if (lineno >= 0)
//			listing.Add(thisLabel, lineno, preProcessor.InMacro());
        if (realName == "..start")
            startupLabel = label;
    }
    if (GetKeyword() == Lexer::colon)
    {
        NextToken();
        thisLabel = NULL;
    }
}
void AsmFile::DoDB()
{
    char buf[3000];
    int size = 0;
    bool byte = GetKeyword() == Lexer::DB;
    short val = 0;
    std::deque<Fixup *> fixups;
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
                for (int i=0; i < len; i++)
                {
                    buf[size++] = str[i];
                }
            }
            else
            {
                char temp[1000];
                int j;
                for (j=0; j < len; j++)
                    temp[j] = str[j];
                temp[j] = 0;
                for (int i=0; i < len;)
                {
                    int v = UTF8::Decode(temp + i);
                    if (v < 0x10000)
                    {
                        (*(short *)(buf + size)) = v;
                        size += 2;
                    }
                    else
                    {
                        v -= 0x10000;
                        int n1 = v >> 10;
                        int n2 = v & 0x3ff;
                        *((short *)(buf+size)) = n1 + 0xd800;
                        *((short *)(buf+size+2)) = n2 + 0xdc00;
                        size += 4;
                                            
                    }
                    i += UTF8::CharSpan(temp + i);
                }
            }
        }
        else
        {
            AsmExprNode *num = GetNumber();
            Fixup *f = new Fixup(num, byte ? 1 : 2, false, 0);
            f->SetInsOffs(size);
            f->SetFileName(errFile);
            f->SetErrorLine(errLine);
            fixups.push_back(f);
            buf[size++] = 0;
            if (!byte)
                buf[size++] = 0;
        }
    } while (GetKeyword() == Lexer::comma);
    Instruction *ins = new Instruction((unsigned char *)buf, size, true);
    if (lineno >= 0)
        listing.Add(ins, lineno, preProcessor.InMacro());
    currentSection->InsertInstruction(ins);
    while (fixups.size())
    {
        Fixup *f = fixups.front();
        fixups.pop_front();
        ins->Add(f);
    }
}
void AsmFile::DoDD()
{
    char buf[3000];
    int size = 0;
    NeedSection();
    int val = 0;
    int lineno = preProcessor.GetMainLineNo();
    std::deque<Fixup *> fixups;
    do
    {
        int errLine = Errors::GetErrorLine();
        std::string errFile = Errors::GetFileName();
        AsmExprNode *num;
        NextToken();
        if (IsString())
        {
            std::wstring str = GetString();
            int len = str.size();
            int i,j;
            char temp[1000];
            for (j=0; j < len; j++)
                temp[j] = str[j];
            temp[j] = 0;
            for (i=0; i < len;)
            {
                int v = UTF8::Decode(temp + i);
                *((unsigned *)(buf+size)) = v;
                size += 4;
                i+= UTF8::CharSpan(temp + i);
            }
        }
        else
        {
            num = GetNumber();
            Fixup *f = new Fixup(num, 4, false, 0);
            f->SetInsOffs(size);
            f->SetFileName(errFile);
            f->SetErrorLine(errLine);
            fixups.push_back(f);
            *((unsigned *)(buf+size)) = 0;
            size += 4;
        }
    } while (GetKeyword() == Lexer::comma);
    Instruction *ins = new Instruction((unsigned char *)buf, size, true);
    if (lineno >= 0)
        listing.Add(ins, lineno, preProcessor.InMacro());
    currentSection->InsertInstruction(ins);
    while (fixups.size())
    {
        Fixup *f = fixups.front();
        fixups.pop_front();
        ins->Add(f);
    }
}
void AsmFile::DoFloat()
{
    unsigned char buf[4000];
    int size = 0;
    bool tbyte = GetKeyword() == Lexer::DT;
    NeedSection();
    int lineno = preProcessor.GetMainLineNo();
    std::deque<Fixup *> fixups;
    memset(buf, 0, sizeof(buf));
    do
    {
        int errLine = Errors::GetErrorLine();
        std::string errFile = Errors::GetFileName();
        int lineno = preProcessor.GetMainLineNo();		
        AsmExprNode *num;
        NextToken();
        num = GetNumber();
        Fixup *f = new Fixup(num, tbyte ? 10 : 8, false, 0);
        f->SetInsOffs(size);
        f->SetFileName(errFile);
        f->SetErrorLine(errLine);
        fixups.push_back(f);
        size += tbyte ? 10 : 8;
    } while (GetKeyword() == Lexer::comma);
    Instruction *ins = new Instruction((unsigned char *)buf, size, true);
    if (lineno >= 0)
        listing.Add(ins, lineno, preProcessor.InMacro());
    currentSection->InsertInstruction(ins);
    while (fixups.size())
    {
        Fixup *f = fixups.front();
        fixups.pop_front();
        ins->Add(f);
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
    Instruction *ins = new Instruction(num, n);
    if (lineno >= 0)
        listing.Add(ins, lineno, preProcessor.InMacro());
    if (inAbsolute)
    {
        absoluteValue += n * num;
    }
    else
    {
        if (GetKeyword() == Lexer::comma)
        {
            NextToken();
            Fixup *f = new Fixup(GetNumber(), n, false, 0);
            f->SetFileName(errFile);
            f->SetErrorLine(errLine);
            ins->Add(f);
        }
        currentSection->InsertInstruction(ins);
    }
}
void AsmFile::EquDirective()
{
    if (!thisLabel)
        throw new std::runtime_error("Label needed");
    int lineno = preProcessor.GetMainLineNo();
    NextToken();
    AsmExprNode *num = GetNumber();
    int n = 0;
    if (inAbsolute)
        n = absoluteValue;
    else if (currentSection)
        n = currentSection->GetPC();
    num = AsmExpr::ConvertToBased(num, n);
    if (num->IsAbsolute())
    {
        AsmExprNode *num1 = AsmExpr::Eval(num, n);
        delete num;
        num = num1;
    }
    thisLabel->SetOffset(num);
    thisLabel->SetSect(-1);
    if (lineno >= 0)
        listing.Add(thisLabel, lineno, preProcessor.InMacro());
    if (!inAbsolute)
        currentSection->pop_back();
    AsmExpr::SetEqu(thisLabel->GetName(), num);
}
void AsmFile::Directive()
{
    NextToken();
    switch (GetKeyword())
    {
        case Lexer::EQU:
            EquDirective();
            break;
        case Lexer::DB:
        case Lexer::DW:
            NoAbsolute();
            DoDB();
            break;
        case Lexer::DD:
            NoAbsolute();
            DoDD();
            break;
        case Lexer::DQ:
        case Lexer::DT:
            NoAbsolute();
            DoFloat();
            break;
        case Lexer::RESB:
            ReserveDirective(1);
            break;
        case Lexer::RESD:
            ReserveDirective(4);
            break;
        case Lexer::RESQ:
            ReserveDirective(8);
            break;
        case Lexer::REST:
            ReserveDirective(10);
            break;
        case Lexer::RESW:
            ReserveDirective(2);
            break;
        case Lexer::SECTION:
            SectionDirective();
            break;
        case Lexer::ABSOLUTE:
            AbsoluteDirective();
            break;
        case Lexer::PUBLIC:
            PublicDirective();
            break;
        case Lexer::EXTERN:
            ExternDirective();
            break;
        case Lexer::ALIGN:
            AlignDirective();
            break;
        case Lexer::INCBIN:
            NoAbsolute();
            IncbinDirective();
            break;
        case Lexer::IMPORT:
            NoAbsolute();
            ImportDirective();
            break;
        case Lexer::EXPORT:
            NoAbsolute();
            ExportDirective();
            break;
        case Lexer::TIMES:
            NoAbsolute();
            TimesDirective();	// timesdirective eats the ']'
            return;
        default:
            throw new std::runtime_error("Expected directive");
    }
    if (GetKeyword() == Lexer::closebr)
    {
        NextToken();
    }
    else
    {
        throw new std::runtime_error("Expected ']'");
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
            absoluteValue += n-v;
    }
    else
    {
        NeedSection();
        Instruction *ins = new Instruction(GetValue());
        currentSection->InsertInstruction(ins);
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
    if (npos != line.size()-1)
    {
        int npos1 = line.find_first_not_of(" \t\v\n\r", npos+1);
        if (npos1 != std::string::npos)
            throw new std::runtime_error("Extra characters on end of line");
    }
    line = line.substr(0, npos);
    lexer.StopAtEol(true);
    for (int i=0; i < n; i++)
    {
        lexer.Reset(line);
        if (GetKeyword() == Lexer::openbr)
        {
            Directive();
        }
        else if (parser->MatchesOpcode(GetToken()->GetChars()))
        {
            Instruction *ins = parser->Parse(lexer.GetRestOfLine(), currentSection->GetPC());
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
    for (int i=0; i < wname.size(); i++)
    {
        char buf[2];
        buf[0] = wname[i];
        buf[1] = 0;
        name = name + buf;
    }
    if (GetKeyword() == Lexer::comma)
    {
        NextToken();
        start = GetValue();
        if (GetKeyword() == Lexer::comma)
        {
            NextToken();
            size = GetValue();
        }
    }
    std::fstream in(name.c_str(), std::ios::in | std::ios::binary);
    if (in == NULL)
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
    unsigned char *data = new unsigned char[size];
    in.read((char *)data, size);
    Instruction *ins = new Instruction(data, size);
    currentSection->InsertInstruction(ins);
    delete[] data;
}
void AsmFile::PublicDirective()
{
    do
    {
        NextToken();
        std::string name = GetId();
        if (caseInsensitive)
        {
            for (int i=0; i < name.size(); i++)
                name[i] = toupper(name[i]);
        }
        globals.insert(name);
    } while (GetKeyword() == Lexer::comma);
}
void AsmFile::ExternDirective()
{
    do
    {
        NextToken();
        std::string name = GetId();
        if (caseInsensitive)
        {
            for (int i=0; i < name.size(); i++)
                name[i] = toupper(name[i]);
        }
        externs.insert(name);
        if (labels[name] != NULL && !labels[name]->IsExtern())
        {
            throw new std::runtime_error(std::string("Label '") + name + "' already exists.");
        }
        else
        {
            Label *label = new Label(name, labels.size(), sections.size()-1);
            label->SetExtern(true);
            labels[name] = label;
            numericLabels.push_back(label);

        }
    } while (GetKeyword() == Lexer::comma);
}
void AsmFile::ImportDirective()
{
    NextToken();
    std::string internal = GetId();
    std::string dll = GetId();
    std::string external;
    if (GetKeyword() != Lexer::closebr)
        external = GetId();
    else
        external = internal;
    Import *imp = new Import;
    imp->dll = dll;
    imp->extname = external;
    imports[internal] = imp;
}
void AsmFile::ExportDirective()
{
    NextToken();
    std::string internal = GetId();
    std::string external;
    if (GetKeyword() != Lexer::closebr)
        external = GetId();
    else
        external = internal;
    exports[internal] = external;
}
void AsmFile::SectionDirective()
{
    NextToken();
    std::string name = GetId();
    if (sections[name] == NULL)
    {
        Section *section = new Section(name, sections.size());
        sections[name] = section;
        numericSections.push_back(section);
        section->Parse(this);
        currentSection = section;
    }
    else
    {
        currentSection = sections[name];
    }
    AsmExpr::SetSection(currentSection);
    parser->Setup(currentSection);
    currentLabel = NULL;
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
void AsmFile::NoAbsolute()
{
    if (inAbsolute)
    {
        throw new std::runtime_error("Code generation in absolute section not allowed");
    }
}
void AsmFile::NeedSection()
{
    if (sections.size() == 0 && !inAbsolute)
    {
        Section *section = new Section("__text__", 1);
        sections["__text__"] = section;
        numericSections.push_back( section);
        currentSection = section;
        AsmExpr::SetSection(currentSection);
        parser->Setup(currentSection);
        currentLabel = NULL;
        AsmExpr::SetCurrentLabel(std::string(""));
    }
}
bool AsmFile::Write(std::string &fileName, std::string &srcName)
{
    bool rv = true;
    ObjIeeeIndexManager im;
    ObjFactory f(&im);
    ObjFile *fi = MakeFile(f, srcName);
    if (fi)
    {
        fi->ResolveSymbols(&f);
        
//        std::fstream out(fileName.c_str(), std::fstream::trunc | std::fstream::out);
        FILE *out = fopen(fileName.c_str(), "wb");
        if (out != NULL)
        {
        
            ObjIeee i(fileName.c_str());
            i.SetTranslatorName(ObjString("oasm"));
            i.SetDebugInfoFlag(false);
        
            if (startSection)
            {
                ObjExpression *left = f.MakeExpression(startSection);
                ObjExpression *right = f.MakeExpression(startupLabel->GetOffset()->ival);
                ObjExpression *sa = f.MakeExpression(ObjExpression::eAdd, left, right);
                i.SetStartAddress(fi, sa);
            }
            i.Write(out, fi, &f);
            fclose(out);
//            out.close();
        }
        else
        {
            Utils::fatal(std::string(std::string("Could not open ") + fileName.c_str() + " for write.").c_str());
        }
    }
    else
    {
        rv = false;
    }
    return rv;
}
ObjFile *AsmFile::MakeFile(ObjFactory &factory, std::string &name)
{
    bool rv = true;
    ObjFile *fi = factory.MakeFile(name);
    if (fi)
    {
        std::time_t x = std::time(0);
        fi->SetFileTime(*std::localtime(&x));
        ObjSourceFile *sf = factory.MakeSourceFile(ObjString(name.c_str()));
        fi->Add(sf);
        for (int i=0; i < numericSections.size(); ++i)
        {
            numericSections[i]->Resolve(this);
            ObjSection *s = numericSections[i]->CreateObject(factory);
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
            for (int i=0; i < numericLabels.size(); ++i)
            {
                Label *l = numericLabels[i];
                l->SetObjectSection(objSections[l->GetSect()]);
                if (l->IsPublic())
                {
                    ObjSymbol *s1 = factory.MakePublicSymbol(l->GetName());
                    ObjExpression *left = factory.MakeExpression(objSections[l->GetSect()]);
                    ObjExpression *right = factory.MakeExpression(l->GetOffset()->ival);
                    ObjExpression *sum = factory.MakeExpression(ObjExpression::eAdd, left, right);
                    s1->SetOffset(sum);
                    fi->Add(s1);
                    l->SetObjSymbol(s1);
                }
                else if (l->IsExtern())
                {
                    ObjSymbol *s1 = factory.MakeExternalSymbol(l->GetName());
                    fi->Add(s1);
                    l->SetObjSymbol(s1);
                }
            }
        }
        for (std::map<ObjString, std::string>::iterator it = exports.begin(); it != exports.end(); ++it)
        {
            ObjExportSymbol *p = factory.MakeExportSymbol(it->first);
            p->SetExternalName(it->second);
            fi->Add(p);			
        }
        for (std::map<ObjString, Import *>::iterator it = imports.begin(); it != imports.end(); ++it)
        {
            ObjImportSymbol *p = factory.MakeImportSymbol(it->first);
            p->SetExternalName(it->second->extname);
            p->SetDllName(it->second->dll);
            fi->Add(p);			
        }
        for (int i=0; i < numericSections.size(); i++)
        {
            if (!numericSections[i]->MakeData(factory, this))
                rv = false;
        }
    }
    if (!rv)
    {
        fi = NULL;
    }
    return fi;
}
bool AsmFile::IsKeyword() 
{
    bool rv = GetToken() && GetToken()->IsKeyword(); 
    return rv;
}
int AsmFile::GetKeyword()
{
    return GetToken()->GetKeyword();
}
unsigned AsmFile::GetTokenId()
{
    unsigned rv = 0;
    if (IsKeyword())
    {
        rv = GetToken()->GetKeyword();
        NextToken();
    }
    return rv;
}
bool AsmFile::IsNumber() 
{
    bool rv = GetToken() && (GetToken()->IsNumeric() || (GetToken()->IsKeyword() && (
        GetToken()->GetKeyword() == Lexer::openpa || GetToken()->GetKeyword() == Lexer::plus ||
        GetToken()->GetKeyword() == Lexer::minus || GetToken()->GetKeyword() == Lexer::not ||
        GetToken()->GetKeyword() == Lexer::compl)));
    return rv;
}
unsigned AsmFile::GetValue()
{
    AsmExprNode *num = GetNumber();
    if (!inAbsolute && !num->IsAbsolute())
        throw new std::runtime_error("Constant value expected");
    int n = 0;
    if (inAbsolute)
        n = absoluteValue;
    else if (currentSection)
        n = currentSection->GetPC();
    AsmExprNode *num1 = AsmExpr::Eval(num, n);
    if (num1->GetType() != AsmExprNode::IVAL)
        throw new std::runtime_error("Integer constant expected");
    int rv = num1->ival;
    delete num;
    delete num1;
    return rv;
}
AsmExprNode *AsmFile::GetNumber()
{
    std::string line = lexer.GetRestOfLine();
    if (currentLabel)
        asmexpr.SetCurrentLabel(currentLabel->GetName());
    AsmExprNode *rv = asmexpr.Build(line);
    if (line == "")
        NextToken();
    else
        lexer.Reset(line);
    return rv;
}
bool AsmFile::IsString() 
{
    return GetToken() && GetToken()->IsString(); 
}
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
bool AsmFile::IsIdentifier() 
{
    return GetToken() && GetToken()->IsIdentifier(); 
}
std::string AsmFile::GetId()
{
    std::string rv;
    if (IsIdentifier())
    {
        rv = GetToken()->GetId();
        if (rv[0] == '%')
            rv.erase(0,1);
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
ObjSection *AsmFile::GetSectionByName(std::string &name)
{
    std::map<std::string, Section *>::iterator it = sections.find(name);
    if (it != sections.end())
        return it->second->GetObjectSection();
    return NULL;
}
