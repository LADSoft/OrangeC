/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

#ifndef AsmFile_H
#define AsmFile_H

#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include "PreProcessor.h"
#include "AsmLexer.h"
#include "ppExpr.h"
#include "InstructionParser.h"
#include "AsmExpr.h"
#include "ObjTypes.h"
#include "Section.h"
#include <stack>

class Token;
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
    AsmFile(PreProcessor& pp, bool CaseInsensitive, bool BinaryOutput, Listing& List, bool GAS, bool NoGASdirectivewarning) :
        preProcessor(pp),
        caseInsensitive(CaseInsensitive),
        binaryOutput(BinaryOutput),
        lexer(pp, GAS),
        asmexpr(nullptr),
        startSection(nullptr),
        startupLabel(nullptr),
        startupSection(nullptr),
        inAbsolute(false),
        currentSection(nullptr),
        currentLabel(nullptr),
        listing(List),
        attSyntax(GAS),
        noGASdirectivewarning(NoGASdirectivewarning),
        absoluteValue(0),
        thisLabel(nullptr),
        bigEndian(false)

    {
        parser = InstructionParser::GetInstance();
        parser->SetATT(attSyntax);
        lexer.SetAsmFile(this);
        lexer.NextToken();
    }
    virtual ~AsmFile();

    std::shared_ptr<Label> Lookup(std::string& name)
    {
        std::shared_ptr<Label> rv = labels[name];
        return rv;
    }
    bool Read();
    bool Write(std::string& fileName, std::string& srcName);
    ObjFile* MakeFile(ObjFactory& factory, std::string& name);
    ObjSection* MakeSection(ObjFactory& factory, std::shared_ptr<Section>& sect);
    void MakeData(ObjFactory& factory, ObjSection* sect, std::shared_ptr<Section>& s);
    const Token* GetToken() { return lexer.GetToken(); }
    bool IsKeyword();
    kw GetKeyword();
    kw GetTokenId();
    bool IsNumber();
    std::shared_ptr<AsmExprNode> GetNumber();
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
    std::shared_ptr<Section> GetCurrentSection() { return currentSection; }

    static std::shared_ptr<Section> GetLabelSection(const ObjString& label)
    {
        auto it = labelSections.find(label);
        if (it != labelSections.end())
            return it->second;
        return nullptr;
    }

  protected:
    void NeedSection();
    void DoPreProcess(int id);
    void DoLabel(std::string& name, int lineno);
    void DoMacro(MacroObject* obj);
    void DoDB();
    void DoDD();
    void DoDQ();
    void DoFloat();
    void ReserveDirective(int n);
    void EquDirective();
    void Directive();
    void AlignDirective();
    void GnuAlignDirective(bool p2);
    void PublicDirective();
    void InsertExtern(const std::string& name);
    void ExternDirective();
    void ResolveAttExterns(const std::set<std::string>& externs);
    void ImportDirective();
    void ExportDirective();
    void SectionDirective();
    void AbsoluteDirective();
    void TimesDirective();
    void IncbinDirective();
    void StringDirective();
    void SingleDirective();
    void DoubleDirective();
    void EqvDirective();
    void SetDirective();
    void AbortDirective();
    void ErrorDirective();
    void WarningDirective();
    void FailDirective();
    void FillDirective();
    void SpaceDirective();
    void NopsDirective();
    void PushsectionDirective();
    void PopsectionDirective();
    void PreviousDirective();
    void SubsectionDirective();
    void GnuSectionDirective();
    void EjectDirective();
    void PrintDirective();
    void TextDirective();
    void DataDirective();
    void UnknownDirective();
    void NoAbsolute();
    void SetSubsection(std::shared_ptr<Section>& sect, int sid);
    void PushSection(const std::string& name, int sid);
    void PopSection();
    void SwapSections();

  private:
    struct SectionPair
    {
        std::shared_ptr<Section> section;
        int subsection;
    };
    bool inAbsolute;
    int absoluteValue;
    bool caseInsensitive;
    std::shared_ptr<Section> currentSection;
    std::shared_ptr<Label> currentLabel;
    std::shared_ptr<Label> thisLabel;
    std::shared_ptr<Section> startupSection;
    std::shared_ptr<Label> startupLabel;
    ObjSection* startSection;
    PreProcessor& preProcessor;
    Lexer lexer;
    AsmExpr asmexpr;
    InstructionParser* parser;
    static std::vector<std::shared_ptr<Section>> numericSections;
    std::unordered_map<ObjString, std::shared_ptr<Label>> labels;
    std::unordered_map<ObjString, std::string> exports;
    std::unordered_map<ObjString, std::unique_ptr<Import>> imports;
    std::vector<std::shared_ptr<Label>> numericLabels;
    std::vector<ObjSection*> objSections;
    std::set<std::string> globals;
    std::set<std::string> externs;
    Listing& listing;
    bool bigEndian;
    bool binaryOutput;
    bool attSyntax;
    bool noGASdirectivewarning;
    std::stack<SectionPair> sectionStack;
    static std::unordered_map<ObjString, std::shared_ptr<Section>> labelSections;
};

#endif