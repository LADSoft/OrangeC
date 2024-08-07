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
    // clang-format off
    enum eType { eLabel, ePublic, eLocal, eGlobal, eExternal, eAuto, eReg, 
            eExport, eImport, eDefinition };
    // clang-format on
    ObjSymbol(const ObjString& Name, eType Type, ObjInt Index) :
        name(Name), offset(nullptr), index(Index), type(Type), baseType(nullptr), sectionRelative(false), sourceFile(nullptr)
    {
    }
    virtual ~ObjSymbol() {}
    bool IsSectionRelative() { return sectionRelative; }
    void SetSectionRelative(bool flag) { sectionRelative = flag; }
    const ObjString& GetName() const { return name; }
    ObjString GetDisplayName();
    void SetName(ObjString& Name) { name = Name; }
    eType GetType() { return type; }
    void SetType(eType Type) { type = Type; }
    ObjType* GetBaseType() { return baseType; }
    void SetBaseType(ObjType* BaseType) { baseType = BaseType; }
    ObjInt GetIndex() { return index; }
    void SetIndex(ObjInt Index) { index = Index; }
    ObjExpression* GetOffset() { return offset; }
    void SetOffset(ObjExpression* Offset) { offset = Offset; }
    ObjSourceFile* GetSourceFile() { return sourceFile; }
    void SetSourceFile(ObjSourceFile* source) { sourceFile = source; }

  private:
    bool sectionRelative;
    eType type;
    ObjString name;
    ObjType* baseType;
    ObjExpression* offset;
    ObjInt index;
    ObjSourceFile* sourceFile;
};

class ObjImportSymbol : public ObjSymbol
{
  public:
    ObjImportSymbol(const ObjString& InternalName) :
        ObjSymbol(InternalName, eImport, 0), byOrdinal(false), ordinal(0), externalName(""), dllName("")
    {
    }
    virtual ~ObjImportSymbol() {}

    bool GetByOrdinal() { return byOrdinal; }
    void SetByOrdinal(bool ByOrdinal) { byOrdinal = ByOrdinal; }
    ObjInt GetOrdinal() { return ordinal; }
    void SetOrdinal(ObjInt Ordinal) { ordinal = Ordinal; }
    ObjString GetExternalName() { return externalName; }
    void SetExternalName(const ObjString& ExternalName) { externalName = ExternalName; }
    ObjString GetDllName() { return dllName; }
    void SetDllName(const ObjString& DllName) { dllName = DllName; }

  private:
    bool byOrdinal;
    ObjString externalName;
    ObjString dllName;
    ObjInt ordinal;
};

class ObjExportSymbol : public ObjSymbol
{
  public:
    ObjExportSymbol(const ObjString& InternalName) :
        ObjSymbol(InternalName, eExport, 0), byOrdinal(false), ordinal(0xffffffff), externalName("")
    {
    }
    virtual ~ObjExportSymbol() {}
    bool GetByOrdinal() { return byOrdinal; }
    void SetByOrdinal(bool ByOrdinal) { byOrdinal = ByOrdinal; }
    ObjInt GetOrdinal() { return ordinal; }
    void SetOrdinal(ObjInt Ordinal) { ordinal = Ordinal; }
    ObjString GetExternalName() { return externalName; }
    void SetExternalName(const ObjString& ExternalName) { externalName = ExternalName; }
    ObjString GetDllName() { return dllName; }
    void SetDllName(ObjString& DllName) { dllName = DllName; }

  protected:
    virtual ObjInt NextIndex(ObjSymbol&) { return 0; }

  private:
    bool byOrdinal;
    ObjInt ordinal;
    ObjString externalName;
    ObjString dllName;
};

class ObjDefinitionSymbol : public ObjSymbol
{
  public:
    ObjDefinitionSymbol(const ObjString& InternalName) : ObjSymbol(InternalName, eDefinition, 0), value(0){};
    virtual ~ObjDefinitionSymbol() {}
    ObjInt GetValue() { return value; }
    void SetValue(ObjInt Val) { value = Val; }

  protected:
    virtual ObjInt NextIndex(ObjSymbol&) { return 0; }

  private:
    ObjInt value;
};
#endif
