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
#ifndef Section_h
#define Section_h

#include <vector>
#include <string>
#include <map>

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
    Section(std::string Name, int Sect) : name(Name), sect(Sect), align(1), pc(0), isVirtual(false) { memset(beValues, 0, sizeof(beValues)); }
    virtual ~Section();
    ObjSection *CreateObject(ObjFactory &factory);
    bool MakeData(ObjFactory &factory, AsmFile *fil);
    void Parse(AsmFile *fil);
    void Resolve(AsmFile *fil);
    void SetAlign(int aln) { align = aln; }
    int GetAlign() { return align; }
    void InsertInstruction(Instruction *ins) { 	instructions.push_back(ins); ins->SetOffset(pc); pc += ins->GetSize(); }	
    Instruction *InsertLabel(Label *label)
    {
        Instruction *l = new Instruction(label);
        instructions.push_back(l);
        labels[label->GetName()] = pc;
        return l;
    }
    void pop_back() { Instruction *v = instructions.back(); delete v; instructions.pop_back(); }
    int GetSect() { return sect; }
    ObjSection *GetObjectSection() { return objectSection; }
    std::string GetName() { return name; }
    int GetNext(Fixup &fixup, unsigned char *buf, int len);
    int beValues[10];
    std::map<std::string, int>::iterator Lookup(std::string name) { return labels.find(name); }
    std::map<std::string, int> & GetLabels() { return labels; }
    int GetPC() { return pc; }
protected:
    ObjExpression *ConvertExpression(AsmExprNode *node, AsmFile *fil, ObjFactory &factory);
    bool SwapSectionIntoPlace(ObjExpression *t);
    void Optimize(AsmFile *fil);
private:
    std::string name;
    int sect;
    int align;
    bool isVirtual;
    std::vector<Instruction *> instructions;
    int instructionPos;
    ObjSection *objectSection;
    int pc;
    std::map<std::string, int> labels;
};
#endif