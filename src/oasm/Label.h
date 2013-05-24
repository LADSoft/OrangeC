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
#ifndef Label_H
#define Label_H

#include <string>
#include "AsmExpr.h"
class ObjSymbol;
class ObjSection;
class Section;

class Label
{
public:
    Label(std::string &Name, int Label, int Sect) : name(Name), label(Label), 
        sect(Sect), publc(false), extrn(false), node(NULL) { }
    ~Label() { if (node) delete node; }
    int GetLabel() { return label; }
    std::string GetName() { return name; }
    AsmExprNode *GetOffset() { return node; }
    void SetOffset(AsmExprNode *Offset) { if (node) delete node; node = Offset; }
    void SetOffset(int offs) { SetOffset(new AsmExprNode(offs)); }
    int GetSect() { return sect; }
    void SetSect(int Sect) { sect = Sect; }
    void SetObjectSection(ObjSection *os) { objSection = os; }
    ObjSection *GetObjectSection() { return objSection; }
    bool IsPublic() { return publc; }
    void SetPublic(bool Publc) { publc = Publc; }
    bool IsExtern() { return extrn; }	
    void SetExtern(bool Extrn) { extrn = Extrn; }
    void SetObjSymbol(ObjSymbol *Sym) { sym = Sym; }
    ObjSymbol *GetObjSymbol() { return sym; }
private:
    std::string name;
    AsmExprNode *node;
    int sect;
    int label;
    bool publc;
    bool extrn;
    ObjSymbol *sym;
    ObjSection *objSection;
};
#endif