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

#ifndef OBJMEMORY_H
#define OBJMEMORY_H
#include <vector>
#include <cstdlib>
#include "ObjTypes.h"
#include <memory>

class ObjExpression;
class ObjSection;
class ObjSymbol;
class ObjDebugTag;
class ObjFactory;

class ObjMemory : public ObjWrapper
{
  public:
    ObjMemory(ObjByte* Data, ObjInt Size) : fixup(nullptr), fill(0), enumerated(false), debugTags(nullptr) { SetData(Data, Size); }
    ObjMemory(ObjExpression* Expression, ObjInt Size) :
        data(nullptr), fill(0), enumerated(false), debugTags(nullptr), fixup(Expression), size(Size)
    {
    }
    ObjMemory(ObjInt Size, ObjInt Fill) :
        data(nullptr), fixup(nullptr), size(Size), fill(Fill), enumerated(true), debugTags(nullptr)
    {
    }
    virtual ~ObjMemory();

    ObjInt GetSize() { return size; }
    ObjByte GetFill() { return fill; }
    bool IsEnumerated() { return enumerated; }
    ObjByte* GetData() { return data.get(); }
    ObjExpression* GetFixup() { return fixup; }
    void SetFixup(ObjExpression* f) { fixup = f; }
    void SetData(ObjByte* Data, ObjInt Size);
    void SetData(ObjExpression* Data, ObjInt Size);
    bool HasDebugTags() { return debugTags != nullptr; }

    typedef std::vector<ObjDebugTag*> DebugTagContainer;
    typedef DebugTagContainer::iterator iterator;
    typedef DebugTagContainer::const_iterator const_iterator;

    iterator begin() { return debugTags->begin(); }
    iterator end() { return debugTags->end(); }
    void SetDebugTags(std::unique_ptr<DebugTagContainer> d)
    {
        if (debugTags)
        {
            for (auto v : *d)
            {
                debugTags->push_back(v);
            }
        }
        else
        {
            debugTags = std::move(d);
        }
    }

  private:
    ObjInt size;
    std::unique_ptr<ObjByte[]> data;
    ObjByte fill;
    ObjExpression* fixup;
    bool enumerated;
    std::unique_ptr<DebugTagContainer> debugTags;
};

class ObjMemoryManager : public ObjWrapper
{
    typedef std::vector<ObjMemory*> MemoryContainer;

  public:
    ObjMemoryManager() : base(0), size(0) {};
    virtual ~ObjMemoryManager() {}
    ObjInt GetBase() { return base; }
    void SetBase(ObjInt Base) { base = Base; }
    ObjInt GetSize() { return size; }
    void Add(ObjMemory* Memory)
    {
        size += Memory->GetSize();
        memory.push_back(Memory);
    }
    void Add(std::unique_ptr<ObjMemory::DebugTagContainer> Tags)
    {
        auto om = new ObjMemory((ObjByte*)nullptr, 0);
        om->SetDebugTags(std::move(Tags));
        memory.push_back(om);
    }
    void ResolveSymbols(ObjFactory* Factory, ObjSection* Section);
    typedef MemoryContainer::iterator iterator;
    typedef MemoryContainer::const_iterator const_iterator;

    iterator begin() { return memory.begin(); }
    iterator end() { return memory.end(); }

  private:
    ObjInt base;
    ObjInt size;
    MemoryContainer memory;
};
#endif
