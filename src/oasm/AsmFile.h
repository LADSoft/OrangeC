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
#ifndef AsmFile_H
#define AsmFile_H

#include <string>
#include <map>
#include <vector>
#include <set>
#include "PreProcessor.h"
#include "AsmLexer.h"
#include "ppExpr.h"
#include "InstructionParser.h"
#include "AsmExpr.h"
#include "ObjTypes.h"
class Token;
class Section;
class Label;
class PreProcessor;
class ObjSection;
class ObjFile;
class ObjFactory;
class MacroObject;
class Listing;

class Import
{
public:
    std::string dll;
    std::string extname;
};

class AsmFile
{
public:
    AsmFile(PreProcessor &pp, bool CaseInsensitive, Listing &List) : preProcessor(pp), caseInsensitive(CaseInsensitive),
        lexer(pp), expr(false, NULL), asmexpr(NULL), startSection(NULL), startupLabel(NULL), startupSection(NULL), inAbsolute(false),
        currentSection(NULL), currentLabel(NULL), listing(List) { parser = InstructionParser::GetInstance(); lexer.SetAsmFile(this); lexer.NextToken(); }
    virtual ~AsmFile();

    Label *Lookup(std::string &name) { 
        Label *rv = labels[name];
        return rv;
    }
    bool Read();
    bool Write(std::string &fileName, std::string &srcName);
    ObjFile *MakeFile(ObjFactory &factory, std::string &name);
    ObjSection *MakeSection(ObjFactory &factory, Section *sect);
    void MakeData(ObjFactory &factory, ObjSection *sect, Section *s);
    const Token *GetToken() { return lexer.GetToken(); }
    bool IsKeyword();
    int GetKeyword();
    unsigned GetTokenId();
    bool IsNumber();
    AsmExprNode *GetNumber();
    bool IsString();
    std::wstring GetString();
    bool IsIdentifier();
    std::string GetId();
    void NeedEol();
    void NextToken() { lexer.NextToken(); }
    bool AtEof() { return lexer.AtEof(); }
    InstructionParser *GetParser() { return parser; }
    unsigned GetValue();
    ObjSection *GetSectionByName(std::string &name);
protected:
    void NeedSection();
    void DoPreProcess(int id);
    void DoLabel(std::string &name, int lineno);
    void DoMacro(MacroObject *obj);
    void DoDB();
    void DoDD();
    void DoFloat();
    void ReserveDirective(int n);
    void EquDirective();
    void Directive();
    void AlignDirective();
    void PublicDirective();
    void ExternDirective();
    void ImportDirective();
    void ExportDirective();
    void SectionDirective();
    void AbsoluteDirective();
    void TimesDirective();
    void IncbinDirective();
    void NoAbsolute();	
private:
    bool inAbsolute;
    int absoluteValue;
    bool caseInsensitive;
    Section *currentSection;
    Label *currentLabel;
    Label *thisLabel;
    Section *startupSection;
    Label *startupLabel;
    ObjSection *startSection;
    PreProcessor &preProcessor;
    Lexer lexer;
    ppExpr expr;
    AsmExpr asmexpr;
    InstructionParser *parser;
    std::map<ObjString, Section *>sections;
    std::vector<Section *> numericSections;
    std::map<ObjString, Label *> labels;
    std::map<ObjString, std::string> exports;
    std::map<ObjString, Import *> imports;
    std::vector<Label *> numericLabels;
    std::vector<ObjSection *> objSections;
    std::set<std::string> globals;
    std::set<std::string> externs;
    Listing &listing;
    bool bigEndian;
} ;

#endif