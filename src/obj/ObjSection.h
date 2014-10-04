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
#ifndef OBJSECTION_H
#define OBJSECTION_H
#include "ObjTypes.h"
#include "ObjIndexManager.h"
#include "ObjMemory.h"
#include "ObjExpression.h"
class ObjLineNo;
class ObjFixup;
class ObjFactory;
class ObjExpression;

class ObjSection : public ObjWrapper
{
public:
    enum eQuals {
        absolute = 1,
        bit = 2,
        common = 4,
        equal = 8 ,
        max = 0x10 ,
        now = 0x20 ,
        postpone = 0x40 ,
        rom = 0x80 ,	
        separate = 0x100 ,
        unique = 0x200 ,
        ram = 0x400 ,
        exec = 0x800 ,
        zero = 0x1000,
        virt = 0x2000 
    } ;
    
    ObjSection(ObjString &Name, ObjInt Index) :name(Name), externalName(""), 
            alignment(1), quals(ram), index(Index), utilityFlag(false), virtualOffset(-1), size(new ObjExpression(0)), offset(new ObjExpression(0)) {}
    virtual ~ObjSection() { };
    ObjString GetName() { return name; }
    void SetName(ObjString &Name) { name = Name; }
    ObjString GetDisplayName();
    ObjString GetExternalName() { return externalName; }
    void SetExternalName(ObjString &Name) {externalName = Name; }
    ObjString GetClass() { return sectionClass; }
    void SetClass(ObjString &SectionClass) { sectionClass = SectionClass; }
    ObjInt GetAlignment() { return alignment; }
    void SetAlignment(ObjInt Alignment) { alignment = Alignment; }
    ObjInt GetQuals() { return quals; }
    void SetQuals(ObjInt Quals) { quals = Quals; }
    ObjInt GetIndex() { return index; }
    void SetIndex(ObjInt Index)	{ index = Index; }
    void Add(ObjMemory *Memory) { memoryManager.Add(Memory); }
    void SetBase(ObjInt Base) { memoryManager.SetBase(Base); }
    ObjInt GetBase() { return memoryManager.GetBase(); }
    ObjInt GetAbsSize() { return memoryManager.GetSize(); }
    void ResolveSymbols(ObjFactory *Factory) { memoryManager.ResolveSymbols(Factory, this); }
    ObjMemoryManager &GetMemoryManager() { return memoryManager; }
    ObjExpression *GetSize() { return size; }
    void SetSize(ObjExpression *exp) { size = exp; }
    ObjExpression *GetOffset() { return offset; }
    void SetOffset(ObjExpression *exp) { offset = exp; }
    ObjInt GetVirtualOffset() { return virtualOffset; }
    void SetVirtualOffset(ObjInt offset) { virtualOffset = offset; }
    bool GetUtilityFlag() { return utilityFlag; }
    void SetUtilityFlag(bool flag) { utilityFlag = flag; }
private:
    ObjString name;
    ObjString externalName;
    ObjInt alignment;
    ObjInt quals;
    ObjInt index;
    ObjExpression *size; /* not supported */
    ObjExpression *offset; /* not supported */
    ObjMemoryManager memoryManager;
    ObjString sectionClass;
    bool utilityFlag;
    ObjInt genOffset;
    ObjInt virtualOffset;
} ;
#endif