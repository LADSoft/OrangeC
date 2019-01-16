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
    AsmFile(PreProcessor& pp, bool CaseInsensitive, bool BinaryOutput, Listing& List) :
        preProcessor(pp),
        caseInsensitive(CaseInsensitive),
        binaryOutput(BinaryOutput),
        lexer(pp),
        asmexpr(nullptr),
        startSection(nullptr),
        startupLabel(nullptr),
        startupSection(nullptr),
        inAbsolute(false),
        currentSection(nullptr),
        currentLabel(nullptr),
        listing(List)
    {
        parser = InstructionParser::GetInstance();
        lexer.SetAsmFile(this);
        lexer.NextToken();
    }
    virtual ~AsmFile();

    Label* Lookup(std::string& name)
    {
        Label* rv = labels[name];
        return rv;
    }
    bool Read();
    bool Write(std::string& fileName, std::string& srcName);
    ObjFile* MakeFile(ObjFactory& factory, std::string& name);
    ObjSection* MakeSection(ObjFactory& factory, Section* sect);
    void MakeData(ObjFactory& factory, ObjSection* sect, Section* s);
    const Token* GetToken() { return lexer.GetToken(); }
    bool IsKeyword();
    int GetKeyword();
    unsigned GetTokenId();
    bool IsNumber();
    AsmExprNode* GetNumber();
    bool IsString();
    std::wstring GetString();
    bool IsIdentifier();
    std::string GetId();
    void NeedEol();
    void NextToken() { lexer.NextToken(); }
    bool AtEof() { return lexer.AtEof(); }
    InstructionParser* GetParser() { return parser; }
    unsigned GetValue();
    ObjSection* GetSectionByName(std::string& name);
    Section* GetCurrentSection() { return currentSection; }

  protected:
    void NeedSection();
    void DoPreProcess(int id);
    void DoLabel(std::string& name, int lineno);
    void DoMacro(MacroObject* obj);
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
    Section* currentSection;
    Label* currentLabel;
    Label* thisLabel;
    Section* startupSection;
    Label* startupLabel;
    ObjSection* startSection;
    PreProcessor& preProcessor;
    Lexer lexer;
    AsmExpr asmexpr;
    InstructionParser* parser;
    std::map<ObjString, Section*> sections;
    std::vector<Section*> numericSections;
    std::map<ObjString, Label*> labels;
    std::map<ObjString, std::string> exports;
    std::map<ObjString, Import*> imports;
    std::vector<Label*> numericLabels;
    std::vector<ObjSection*> objSections;
    std::set<std::string> globals;
    std::set<std::string> externs;
    Listing& listing;
    bool bigEndian;
    bool binaryOutput;
};

#endif