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
#ifndef OBJMEMORY_H
#define OBJMEMORY_H
#include <vector>
#include <cstdlib>
#include "ObjTypes.h"

class ObjExpression;
class ObjSection;
class ObjSymbol;
class ObjDebugTag;
class ObjFactory;

class ObjMemory : public ObjWrapper
{
public:
    ObjMemory(ObjByte *Data, ObjInt Size) : fixup(nullptr), data(nullptr), fill(0), enumerated(false), debugTags(nullptr)
    {
        SetData(Data, Size);
    }
    ObjMemory(ObjExpression *Expression, ObjInt Size) : data(nullptr), fill(0), enumerated(false), debugTags(nullptr),
            fixup(Expression), size(Size) {}
    ObjMemory(ObjInt Size, ObjInt Fill) : data(nullptr), fixup(nullptr), size(Size), fill(Fill), enumerated(true), debugTags(nullptr) { }
    virtual ~ObjMemory() { delete debugTags; };
    
    ObjInt GetSize() { return size; }
    ObjByte GetFill() { return fill; }
    bool IsEnumerated() { return enumerated; }
    ObjByte *GetData() { return data; }
    ObjExpression *GetFixup() { return fixup; }
    void SetFixup(ObjExpression *f) { fixup = f; }
    void SetData(ObjByte *Data, ObjInt Size);
    void SetData(ObjExpression *Data, ObjInt Size);
    bool HasDebugTags() { return debugTags != nullptr; }

    typedef std::vector<ObjDebugTag *> DebugTagContainer;
    typedef DebugTagContainer::iterator DebugTagIterator;
    typedef DebugTagContainer::const_iterator const_DebugTagIterator;

    DebugTagIterator DebugTagBegin() { return debugTags->begin(); }
    DebugTagIterator DebugTagEnd() { return debugTags->end(); }
    void SetDebugTags(DebugTagContainer *d)
    {
        if (debugTags)
        {
            for (int i=0; i < d->size(); i++)
            {
                debugTags->push_back((*d)[i]);
            }
            delete d;
        }
        else
        {
            debugTags = d;
        }
    }
private:
    ObjInt size;
    ObjByte *data;
    ObjByte fill;
    ObjExpression *fixup;
    bool enumerated;
    DebugTagContainer *debugTags;
} ;

class ObjMemoryManager : public ObjWrapper
{
    typedef std::vector<ObjMemory *> MemoryContainer;
public:
    ObjMemoryManager() : base(0), size(0) {};
    virtual ~ObjMemoryManager() { }
    ObjInt GetBase() { return base; }
    void SetBase(ObjInt Base) { base = Base; }
    ObjInt GetSize() { return size; }
    void Add(ObjMemory *Memory)
    {
        size += Memory->GetSize();
        memory.push_back(Memory);
    }
    void Add(ObjMemory::DebugTagContainer *Tags)
    {
        if (memory.size())
            memory[memory.size()-1]->SetDebugTags(Tags);
    }
    void ResolveSymbols(ObjFactory *Factory, ObjSection *Section);
    typedef MemoryContainer::iterator MemoryIterator;
    typedef MemoryContainer::const_iterator const_MemoryIterator;

    MemoryIterator MemoryBegin() { return memory.begin(); }
    MemoryIterator MemoryEnd() { return memory.end(); }
private:
    ObjInt base;
    ObjInt size;
    MemoryContainer memory;
} ;
#endif
