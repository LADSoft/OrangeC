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
#ifndef OBJSYMBOL_H
#define OBJSYMBOL_H

#include "ObjTypes.h"
#include "ObjIndexManager.h"

class ObjType;
class ObjExpression;
class ObjSourceFile;

class ObjSymbol : public ObjWrapper
{
public:
    enum eType { eLabel, ePublic, eLocal, eGlobal, eExternal, eAuto, eReg, 
            eExport, eImport, eDefinition };
    ObjSymbol(const ObjString &Name, eType Type, ObjInt Index) : name(Name), 
            offset(0), index(Index), type(Type), baseType(NULL),
            sectionRelative(false) { }
    virtual ~ObjSymbol() { }
    bool IsSectionRelative() { return sectionRelative; }
    void SetSectionRelative(bool flag) { sectionRelative = flag; }
    ObjString &GetName() { return name; }
    void SetName(ObjString &Name ) {name = Name; }
    eType GetType() { return type; }
    void SetType(eType Type) { type = Type; }
    ObjType *GetBaseType() { return baseType; }
    void SetBaseType(ObjType *BaseType) { baseType = BaseType; }
    ObjInt GetIndex() { return index; }
    void SetIndex(ObjInt Index) { index = Index; }
    ObjExpression *GetOffset() { return offset; }
    void SetOffset(ObjExpression *Offset) { offset = Offset; }
    ObjSourceFile *GetSourceFile() { return sourceFile; }
    void SetSourceFile(ObjSourceFile *source) { sourceFile = source; }
private:
    bool sectionRelative;
    eType type;
    ObjString name;
    ObjType *baseType;
    ObjExpression *offset;
    ObjInt index;
    ObjSourceFile *sourceFile;
} ;

class ObjImportSymbol : public ObjSymbol
{
public:
    ObjImportSymbol(const ObjString &InternalName) : ObjSymbol(InternalName, eImport, 0),
        byOrdinal(false), ordinal(0), externalName(""), dllName("") {} 
    virtual ~ObjImportSymbol() {}
        
    bool GetByOrdinal() { return byOrdinal; }
    void SetByOrdinal(bool ByOrdinal) { byOrdinal = ByOrdinal; }
    ObjInt GetOrdinal() { return ordinal; }
    void SetOrdinal(ObjInt Ordinal) { ordinal = Ordinal; }
    ObjString GetExternalName() { return externalName; }
    void SetExternalName(ObjString &ExternalName) { externalName = ExternalName; }
    ObjString GetDllName() { return dllName; }
    void SetDllName(ObjString &DllName) { dllName = DllName; }
private:
    bool byOrdinal;
    ObjString externalName;
    ObjString dllName;
    ObjInt ordinal;
} ;

class ObjExportSymbol : public ObjSymbol
{
public:
    ObjExportSymbol(const ObjString &InternalName) : ObjSymbol(InternalName, eExport, 0),
        byOrdinal(false), ordinal(0xffffffff), externalName("") {} 
    virtual ~ObjExportSymbol() {}
    bool GetByOrdinal() { return byOrdinal; }
    void SetByOrdinal(bool ByOrdinal) { byOrdinal = ByOrdinal; }
    ObjInt GetOrdinal() { return ordinal; }
    void SetOrdinal(ObjInt Ordinal) { ordinal = Ordinal; }
    ObjString GetExternalName() { return externalName; }
    void SetExternalName(ObjString &ExternalName) { externalName = ExternalName; }
    ObjString GetDllName() { return dllName; }
    void SetDllName(ObjString &DllName) { dllName = DllName; }
protected:
    virtual ObjInt NextIndex(ObjSymbol &)
    {
        return 0;
    }
private:
    bool byOrdinal;
    ObjInt ordinal;
    ObjString externalName;
    ObjString dllName;
} ;

class ObjDefinitionSymbol : public ObjSymbol
{
public:
    ObjDefinitionSymbol(const ObjString &InternalName) : ObjSymbol(InternalName, eDefinition, 0),
        value(0) { };
    virtual ~ObjDefinitionSymbol() {}
    ObjInt GetValue() { return value; }
    void SetValue(ObjInt Val) { value = Val; }
protected:
    virtual ObjInt NextIndex(ObjSymbol &)
    {
        return 0;
    }
private:
    ObjInt value;
} ;
#endif
