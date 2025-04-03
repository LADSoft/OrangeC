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
    typedef std::vector<ObjWrapper*> MemoryContainer;

  public:
    ObjFactory(ObjIndexManager* IndexManager) : indexManager(IndexManager) {}
    ~ObjFactory() { Deallocate(); }
    ObjIndexManager* GetIndexManager() { return indexManager; }
    virtual ObjBrowseInfo* MakeBrowseInfo(ObjBrowseInfo::eType Type, ObjBrowseInfo::eQual Qual, ObjLineNo* Line, ObjInt CharPos,
                                          const ObjString& Data)
    {
        ObjBrowseInfo* b = new ObjBrowseInfo(Type, Qual, Line, CharPos, Data);
        Tag(b);
        return b;
    }
    virtual ObjDebugTag* MakeDebugTag(ObjLineNo* LineNo)
    {
        ObjDebugTag* d = new ObjDebugTag(LineNo);
        Tag(d);
        return d;
    }
    virtual ObjDebugTag* MakeDebugTag(ObjSymbol* Symbol)
    {
        ObjDebugTag* d = new ObjDebugTag(Symbol);
        Tag(d);
        return d;
    }
    virtual ObjDebugTag* MakeDebugTag(ObjSection* Section, bool Start)
    {
        ObjDebugTag* d = new ObjDebugTag(Section, Start);
        Tag(d);
        return d;
    }
    virtual ObjDebugTag* MakeDebugTag(ObjSymbol* Function, bool Start)
    {
        ObjDebugTag* d = new ObjDebugTag(Function, Start);
        Tag(d);
        return d;
    }
    virtual ObjDebugTag* MakeDebugTag(bool Start)
    {
        ObjDebugTag* d = new ObjDebugTag(Start);
        Tag(d);
        return d;
    }
    virtual ObjExpression* MakeExpression(ObjExpression::eOperator Type)
    {
        ObjExpression* e = new ObjExpression(Type);
        Tag(e);
        return e;
    }
    virtual ObjExpression* MakeExpression(ObjInt Left)
    {
        ObjExpression* e = new ObjExpression(Left);
        Tag(e);
        return e;
    }
    virtual ObjExpression* MakeExpression(ObjExpression* Left)
    {
        ObjExpression* e = new ObjExpression(Left);
        Tag(e);
        return e;
    }
    virtual ObjExpression* MakeExpression(ObjSymbol* Left)
    {
        ObjExpression* e = new ObjExpression(Left);
        Tag(e);
        return e;
    }
    virtual ObjExpression* MakeExpression(ObjSection* Left)
    {
        ObjExpression* e = new ObjExpression(Left);
        Tag(e);
        return e;
    }
    virtual ObjExpression* MakeExpression(ObjExpression::eOperator op, ObjExpression* Left)
    {
        ObjExpression* e = new ObjExpression(op, Left);
        Tag(e);
        return e;
    }
    virtual ObjExpression* MakeExpression(ObjExpression::eOperator op, ObjExpression* Left, ObjExpression* Right)
    {
        ObjExpression* e = new ObjExpression(op, Left, Right);
        Tag(e);
        return e;
    }
    virtual ObjFile* MakeFile(const ObjString& Name)
    {
        ObjFile* f = new ObjFile(Name);
        Tag(f);
        return f;
    }
    virtual ObjFunction* MakeFunction(const ObjString& Name, ObjType* ReturnType, int index = -1)
    {
        ObjFunction* f = new ObjFunction(Name, ReturnType, index >= 0 ? index : indexManager->NextType());
        Tag(f);
        return f;
    }
    virtual ObjLineNo* MakeLineNo(ObjSourceFile* File, ObjInt LineNumber)
    {
        ObjLineNo* l = new ObjLineNo(File, LineNumber);
        Tag(l);
        return l;
    }
    virtual ObjMemory* MakeData(ObjByte* Data, ObjInt Size)
    {
        ObjMemory* m = new ObjMemory(Data, Size);
        Tag(m);
        return m;
    }
    virtual ObjMemory* MakeData(ObjInt Size, ObjByte fill)
    {
        ObjMemory* m = new ObjMemory(Size, fill);
        Tag(m);
        return m;
    }
    virtual ObjMemory* MakeFixup(ObjExpression* Expression, ObjInt Size)
    {
        ObjMemory* m = new ObjMemory(Expression, Size);
        Tag(m);
        return m;
    }
    virtual ObjSection* MakeSection(const ObjString& Name, int index = -1)
    {
        ObjSection* s = new ObjSection(Name, index >= 0 ? index : indexManager->NextSection());
        Tag(s);
        return s;
    }
    virtual ObjSourceFile* MakeSourceFile(const ObjString& Name, int index = -1)
    {
        ObjSourceFile* s = new ObjSourceFile(Name, index >= 0 ? index : indexManager->NextFile());
        Tag(s);
        return s;
    }
    virtual ObjSymbol* MakePublicSymbol(const ObjString& Name, int index = -1)
    {
        ObjSymbol* p = new ObjSymbol(Name, ObjSymbol::ePublic, index >= 0 ? index : indexManager->NextPublic());
        p->SetSectionRelative(true);
        Tag(p);
        return p;
    }
    virtual ObjSymbol* MakeExternalSymbol(const ObjString& Name, int index = -1)
    {
        ObjSymbol* p = new ObjSymbol(Name, ObjSymbol::eExternal, index >= 0 ? index : indexManager->NextExternal());
        Tag(p);
        return p;
    }
    virtual ObjSymbol* MakeLocalSymbol(const ObjString& Name, int index = -1)
    {
        ObjSymbol* p = new ObjSymbol(Name, ObjSymbol::eLocal, index >= 0 ? index : indexManager->NextLocal());
        p->SetSectionRelative(true);
        Tag(p);
        return p;
    }
    virtual ObjSymbol* MakeAutoSymbol(const ObjString& Name, int index = -1)
    {
        ObjSymbol* p = new ObjSymbol(Name, ObjSymbol::eAuto, index >= 0 ? index : indexManager->NextAuto());
        Tag(p);
        return p;
    }
    virtual ObjSymbol* MakeRegSymbol(const ObjString& Name, int index = -1)
    {
        ObjSymbol* p = new ObjSymbol(Name, ObjSymbol::eReg, index >= 0 ? index : indexManager->NextReg());
        Tag(p);
        return p;
    }
    virtual ObjSymbol* MakeLabelSymbol(const ObjString& Name)
    {
        ObjSymbol* p = new ObjSymbol(Name, ObjSymbol::eLabel, 0);
        p->SetSectionRelative(true);
        Tag(p);
        return p;
    }
    virtual ObjImportSymbol* MakeImportSymbol(const ObjString Name)
    {
        ObjImportSymbol* p = new ObjImportSymbol(Name);
        Tag(p);
        return p;
    }
    virtual ObjExportSymbol* MakeExportSymbol(const ObjString Name)
    {
        ObjExportSymbol* p = new ObjExportSymbol(Name);
        Tag(p);
        return p;
    }
    virtual ObjDefinitionSymbol* MakeDefinitionSymbol(const ObjString& Name)
    {
        ObjDefinitionSymbol* p = new ObjDefinitionSymbol(Name);
        Tag(p);
        return p;
    }
    virtual ObjType* MakeType(ObjType::eType Type, ObjInt Index = -1)
    {
        ObjType* t;
        if (Index == -1 && Type < ObjType::eVoid)
            t = new ObjType(Type, indexManager->NextType());
        else
            t = new ObjType(Type, Index);
        Tag(t);
        return t;
    }
    virtual ObjType* MakeType(ObjType::eType Type, ObjType* Base, ObjInt Index = -1)
    {
        ObjType* t;
        if (Index == -1 && Type < ObjType::eVoid)
            t = new ObjType(Type, Base, indexManager->NextType());
        else
            t = new ObjType(Type, Base, Index);
        Tag(t);
        return t;
    }
    virtual ObjType* MakeType(const ObjString& Name, ObjType::eType Type, ObjType* Base, ObjInt Index = -1)
    {
        ObjType* t;
        if (Index == -1 && Type < ObjType::eVoid)
            t = new ObjType(Name, Type, Base, indexManager->NextType());
        else
            t = new ObjType(Name, Type, Base, Index);
        Tag(t);
        return t;
    }
    virtual ObjField* MakeField(const ObjString& Name, ObjType* Base, ObjInt ConstVal, ObjInt index)
    {
        ObjField* f = new ObjField(Name, Base, ConstVal, index);
        Tag(f);
        return f;
    }
    typedef MemoryContainer::iterator iterator;
    typedef MemoryContainer::const_iterator const_iterator;

    iterator begin() { return allocated.begin(); }
    iterator end() { return allocated.end(); }

  protected:
    virtual void Deallocate();
    virtual void Tag(ObjWrapper* obj);

  private:
    ObjIndexManager* indexManager;
    MemoryContainer allocated;
};
#endif