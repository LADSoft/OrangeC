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
#ifndef OBJFACTORY_H
#define OBJFACTORY_H
#include "ObjTypes.h"
#include "ObjBrowseInfo.h"
#include "ObjDebugTag.h"
#include "ObjExpression.h"
#include "ObjFile.h"
#include "ObjFunction.h"
#include "ObjIndexManager.h"
#include "ObjLineNo.h"
#include "ObjMemory.h"
#include "ObjSection.h"
#include "ObjSourceFile.h"
#include "ObjSymbol.h"
#include "ObjType.h"

class ObjFactory 
{
    typedef std::vector<ObjWrapper *> MemoryContainer;
public:
    ObjFactory(ObjIndexManager *IndexManager) : indexManager(IndexManager) { }
    ~ObjFactory() { Deallocate(); }
    ObjIndexManager *GetIndexManager() { return indexManager; }
    virtual ObjBrowseInfo *MakeBrowseInfo(ObjBrowseInfo::eType Type, ObjBrowseInfo::eQual Qual, 
                                          ObjLineNo *Line, ObjInt CharPos, ObjString &Data)
    {
        ObjBrowseInfo *b = new ObjBrowseInfo(Type, Qual, Line, CharPos, Data);
        Tag(b);
        return b;
    }
    virtual ObjDebugTag *MakeDebugTag(ObjLineNo *LineNo)
    {
        ObjDebugTag *d = new ObjDebugTag(LineNo);
        Tag(d);
        return d;
    }
    virtual ObjDebugTag *MakeDebugTag(ObjSymbol *Symbol)
    {
        ObjDebugTag *d = new ObjDebugTag(Symbol);
        Tag(d);
        return d;
    }
    virtual ObjDebugTag *MakeDebugTag(ObjSymbol *Function, bool Start)
    {
        ObjDebugTag *d = new ObjDebugTag(Function, Start);
        Tag(d);
        return d;
    }
    virtual ObjDebugTag *MakeDebugTag(bool Start)
    {
        ObjDebugTag *d = new ObjDebugTag(Start);
        Tag(d);
        return d;
    }
    virtual ObjExpression *MakeExpression(ObjExpression::eOperator Type)
    {
        ObjExpression *e = new ObjExpression(Type);
        Tag(e);
        return e;
    }
    virtual ObjExpression *MakeExpression(ObjInt Left)
    {
        ObjExpression *e = new ObjExpression(Left);
        Tag(e);
        return e;
    }
    virtual ObjExpression *MakeExpression(ObjExpression *Left)
    {
        ObjExpression *e = new ObjExpression(Left);
        Tag(e);
        return e;
    }
    virtual ObjExpression *MakeExpression(ObjSymbol *Left)
    {
        ObjExpression *e = new ObjExpression(Left);
        Tag(e);
        return e;
    }
    virtual ObjExpression *MakeExpression(ObjSection *Left)
    {
        ObjExpression *e = new ObjExpression(Left);
        Tag(e);
        return e;
    }
    virtual ObjExpression *MakeExpression(ObjExpression::eOperator op, 
                                          ObjExpression *Left)
    {
        ObjExpression *e = new ObjExpression(op, Left);
        Tag(e);
        return e;
    }
    virtual ObjExpression *MakeExpression(ObjExpression::eOperator op, 
                                          ObjExpression *Left,
                                          ObjExpression *Right)
    {
        ObjExpression *e = new ObjExpression(op, Left, Right);
        Tag(e);
        return e;
    }
    virtual ObjFile *MakeFile(ObjString &Name)
    {
        ObjFile *f = new ObjFile(Name);
        Tag(f);
        return f;
    }
    virtual ObjFunction *MakeFunction(ObjString &Name, ObjType *ReturnType, int index = -1)
    {
        ObjFunction *f = new ObjFunction(Name, ReturnType, index >= 0 ? index : indexManager->NextType());
        Tag(f);
        return f;
    }
    virtual ObjLineNo *MakeLineNo(ObjSourceFile *File, ObjInt LineNumber)
    {
        ObjLineNo *l = new ObjLineNo(File , LineNumber);
        Tag(l);
        return l;
    }
    virtual ObjMemory *MakeData(ObjByte *Data, ObjInt Size)
    {
        ObjMemory *m = new ObjMemory(Data, Size);
        Tag(m);
        return m;
    }
    virtual ObjMemory *MakeData(ObjInt Size, ObjByte fill)
    {
        ObjMemory *m = new ObjMemory(Size, fill);
        Tag(m);
        return m;
    }
    virtual ObjMemory *MakeFixup(ObjExpression *Expression, ObjInt Size)
    {
        ObjMemory *m = new ObjMemory(Expression, Size);
        Tag(m);
        return m;
    }
    virtual ObjSection *MakeSection(ObjString &Name, int index = -1)
    {
        ObjSection *s = new ObjSection(Name, index >= 0 ? index : indexManager->NextSection());
        Tag(s);
        return s;
    }
    virtual ObjSourceFile *MakeSourceFile(ObjString &Name, int index = -1)
    {
        ObjSourceFile *s = new ObjSourceFile(Name, index >= 0 ? index : indexManager->NextFile());
        Tag (s);
        return s;
    }
    virtual ObjSymbol *MakePublicSymbol(ObjString &Name, int index=-1)
    {
        ObjSymbol *p = new ObjSymbol(Name, ObjSymbol::ePublic, 
                                     index >= 0 ? index : indexManager->NextPublic());
        p->SetSectionRelative(true);
        Tag(p);
        return p;
    }
    virtual ObjSymbol *MakeExternalSymbol(ObjString &Name, int index = -1)
    {
        ObjSymbol *p = new ObjSymbol(Name, ObjSymbol::eExternal, 
                                     index >= 0 ? index : indexManager->NextExternal());
        Tag(p);
        return p;
    }
    virtual ObjSymbol *MakeLocalSymbol(ObjString &Name, int index = -1)
    {
        ObjSymbol *p = new ObjSymbol(Name, ObjSymbol::eLocal, 
                                     index >= 0 ? index : indexManager->NextLocal());
        p->SetSectionRelative(true);
        Tag(p);
        return p;
    }
    virtual ObjSymbol *MakeAutoSymbol(ObjString &Name, int index = -1)
    {
        ObjSymbol *p = new ObjSymbol(Name, ObjSymbol::eAuto, 
                                     index >= 0 ? index : indexManager->NextAuto());
        Tag(p);
        return p;
    }
    virtual ObjSymbol *MakeRegSymbol(ObjString &Name, int index = -1)
    {
        ObjSymbol *p = new ObjSymbol(Name, ObjSymbol::eReg, 
                                     index >= 0 ? index : indexManager->NextReg());
        Tag(p);
        return p;
    }
    virtual ObjSymbol *MakeLabelSymbol(ObjString &Name)
    {
        ObjSymbol *p = new ObjSymbol(Name, ObjSymbol::eLabel, 0);
        p->SetSectionRelative(true);
        Tag(p);
        return p;
    }
    virtual ObjImportSymbol *MakeImportSymbol(const ObjString &Name)
    {
        ObjImportSymbol *p = new ObjImportSymbol(Name);
        Tag(p);
        return p;
    }
    virtual ObjExportSymbol *MakeExportSymbol(const ObjString &Name)
    {
        ObjExportSymbol *p = new ObjExportSymbol(Name);
        Tag(p);
        return p;
    }
    virtual ObjDefinitionSymbol *MakeDefinitionSymbol(ObjString &Name)
    {
        ObjDefinitionSymbol *p = new ObjDefinitionSymbol(Name);
        Tag(p);
        return p;
    }
    virtual ObjType *MakeType(ObjType::eType Type, ObjInt Index = -1)
    {
        ObjType *t;
        if (Index == -1 && Type < ObjType::eVoid)
            t = new ObjType(Type, indexManager->NextType());
        else
            t = new ObjType(Type, Index);
        Tag(t);
        return t;
    }
    virtual ObjType *MakeType(ObjType::eType Type, ObjType *Base, ObjInt Index = -1)
    {
        ObjType *t;
        if (Index == -1 && Type < ObjType::eVoid)
            t = new ObjType(Type, Base, indexManager->NextType());
        else
            t = new ObjType(Type, Base, Index);
        Tag(t);
        return t;
    }
    virtual ObjType *MakeType(ObjString &Name, ObjType::eType Type, ObjType *Base, ObjInt Index = -1)
    {
        ObjType *t;
        if (Index == -1 && Type < ObjType::eVoid)
            t = new ObjType(Name, Type, Base, indexManager->NextType());
        else
            t = new ObjType(Name, Type, Base, Index);
        Tag(t);
        return t;
    }
    virtual ObjField *MakeField(ObjString &Name, ObjType *Base, ObjInt ConstVal)
    {
        ObjField *f = new ObjField(Name, Base, ConstVal);
        Tag(f);
        return f;
    }
    typedef MemoryContainer::iterator MemoryIterator;
    typedef MemoryContainer::const_iterator const_MemoryIterator;

    MemoryIterator MemoryBegin() { return allocated.begin(); }
    MemoryIterator MemoryEnd() { return allocated.end(); }

protected:
    virtual void Deallocate();
    virtual void Tag(ObjWrapper *obj);
private:
    ObjIndexManager *indexManager;
    MemoryContainer allocated;
        
};
#endif