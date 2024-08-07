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
class ObjType;

class ObjSection : public ObjWrapper
{
  public:
    enum eQuals
    {
        absolute = 1,
        bit = 2,
        common = 4,
        equal = 8,
        max = 0x10,
        now = 0x20,
        postpone = 0x40,
        rom = 0x80,
        separate = 0x100,
        unique = 0x200,
        ram = 0x400,
        exec = 0x800,
        zero = 0x1000,
        virt = 0x2000
    };

    ObjSection(const ObjString& Name, ObjInt Index) :
        name(Name),
        externalName(""),
        alignment(1),
        quals(ram),
        index(Index),
        utilityFlag(false),
        virtualOffset(-1),
        size(nullptr),
        offset(nullptr),
        aliasFor(nullptr),
        used(false),
        virtualSectionType(nullptr),
        genOffset(0)
    {
    }
    virtual ~ObjSection() {}
    ObjString GetName() const { return name; }
    void SetName(ObjString& Name) { name = Name; }
    ObjString GetDisplayName();
    ObjString GetExternalName() { return externalName; }
    void SetExternalName(ObjString& Name) { externalName = Name; }
    ObjString GetClass() { return sectionClass; }
    void SetClass(ObjString& SectionClass) { sectionClass = SectionClass; }
    ObjInt GetAlignment() { return alignment; }
    void SetAlignment(ObjInt Alignment) { alignment = Alignment; }
    ObjInt GetQuals() { return quals; }
    void SetQuals(ObjInt Quals) { quals = Quals; }
    ObjInt GetIndex() { return index; }
    void SetUsed(bool state) { used = state; }
    bool GetUsed() const { return used; }
    void SetIndex(ObjInt Index) { index = Index; }
    void Add(ObjMemory* Memory) { memoryManager.Add(Memory); }
    void Add(std::unique_ptr<ObjMemory::DebugTagContainer> Tags) { memoryManager.Add(std::move(Tags)); }
    void SetBase(ObjInt Base) { memoryManager.SetBase(Base); }
    ObjInt GetBase() { return memoryManager.GetBase(); }
    ObjInt GetAbsSize() { return memoryManager.GetSize(); }
    void SetVirtualType(ObjType* type) { virtualSectionType = type; }
    ObjType* GetVirtualType() const { return virtualSectionType; }
    void ResolveSymbols(ObjFactory* Factory) { memoryManager.ResolveSymbols(Factory, this); }
    ObjMemoryManager& GetMemoryManager() { return memoryManager; }
    ObjExpression* GetSize() { return size; }
    void SetSize(ObjExpression* exp) { size = exp; }
    ObjExpression* GetOffset() { return offset; }
    void SetOffset(ObjExpression* exp) { offset = exp; }
    ObjInt GetVirtualOffset() { return virtualOffset; }
    void SetVirtualOffset(ObjInt offset) { virtualOffset = offset; }
    bool GetUtilityFlag() { return utilityFlag; }
    void SetUtilityFlag(bool flag) { utilityFlag = flag; }
    void SetAliasFor(ObjSection* other) { aliasFor = other; }
    ObjSection* GetAliasFor() const { return aliasFor; }

  private:
    ObjSection* aliasFor;
    ObjString name;
    ObjString externalName;
    ObjInt alignment;
    ObjInt quals;
    ObjInt index;
    ObjExpression* size;   /* not supported */
    ObjExpression* offset; /* not supported */
    ObjMemoryManager memoryManager;
    ObjString sectionClass;
    bool utilityFlag;
    bool used;  // for the linker
    ObjInt genOffset;
    ObjInt virtualOffset;
    ObjType* virtualSectionType;
};
#endif