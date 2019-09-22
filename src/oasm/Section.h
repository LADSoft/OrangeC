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

#ifndef Section_h
#define Section_h

#include <vector>
#include <string>
#include <map>
#include <functional>

#include "Instruction.h"
class Label;
class ObjExpression;
class ObjFactory;
class ObjSection;
class AsmFile;
class Fixup;

class Section
{
  public:
    Section(std::string Name, int Sect) : name(Name), sect(Sect), align(1), pc(0), isVirtual(false)
    {
        memset(beValues, 0, sizeof(beValues));
    }
    virtual ~Section();
    ObjSection* CreateObject(ObjFactory& factory);
    bool MakeData(ObjFactory& factory, std::function<Label*(std::string&)> Lookup,
                  std::function<ObjSection*(std::string&)> SectLookup,
                  std::function<void(ObjFactory&, Section*, Instruction*)> HandleAlt);
    void Parse(AsmFile* fil);
    void Resolve();
    void SetAlign(int aln) { align = aln; }
    int GetAlign() { return align; }
    void InsertInstruction(Instruction* ins);
    Instruction* InsertLabel(Label* label);
    void pop_back();
    std::vector<std::unique_ptr<Instruction>>& GetInstructions() { return instructions; }
    void ClearInstructions() { instructions.clear(); labels.clear();  }
    int GetSect() { return sect; }
    ObjSection* GetObjectSection() { return objectSection; }
    std::string GetName() { return name; }
    int GetNext(Fixup& fixup, unsigned char* buf, int len);
    int beValues[10];
    std::map<std::string, int>::iterator Lookup(std::string& name) { return labels.find(name); }
    std::map<std::string, int>& GetLabels() { return labels; }
    int GetPC() { return pc; }
    bool HasInstructions() const { return instructions.size() != 0; }

    static void NoShowError() { dontShowError = true; }
    int GetSubsection() const { return subSection; }
    void SetSubsection(int sid) 
    { 
        subSection = sid; 
        if (sid != 0)
        {
            if (subsections[sid] == nullptr)
                subsections[sid] = new Section(name, sect);
        }
    }
    void MergeSubsections();
  protected:
    ObjExpression* ConvertExpression(AsmExprNode* node, std::function<Label*(std::string&)> Lookup,
                                     std::function<ObjSection*(std::string&)> SectLookup, ObjFactory& factory);
    bool SwapSectionIntoPlace(ObjExpression* t);
    void Optimize();

  private:
    static bool dontShowError;
    int subSection = 0;
    std::string name;
    int sect;
    int align;
    bool isVirtual;
    std::vector<std::unique_ptr<Instruction>> instructions;
    int instructionPos;
    ObjSection* objectSection;
    int pc;
    std::map<std::string, int> labels;
    std::map<int, Section*> subsections;
};
#endif