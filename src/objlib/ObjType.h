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
#ifndef OBJTYPE_H
#define OBJTYPE_H
#include <vector>
#include "ObjTypes.h"
#include "ObjIndexManager.h"

class ObjField;

class ObjType : public ObjWrapper
{
    typedef std::vector<ObjField *> FieldContainer;
public:
    enum eType { eNone = 0, ePointer, eFunction, eBitField,
                 eStruct, eUnion, eArray, eVla, eEnum,
                 eField, eTypeDef, eLRef, eRRef,
                 eVoid = 32, ePVoid, eBool, eBit,
                 eChar = 40, eShort, eInt, eLong,
                 eLongLong, eWcharT, eChar16T, eChar32T,
                 eUChar = 48, eUShort, eUInt, eULong,
                 eULongLong,
                 ePChar = 56, ePShort, ePInt, ePLong,
                 ePLongLong, ePWcharT, ePChar16T, ePChar32T,
                 ePUChar = 64, ePUShort, ePUInt, ePULong,
                 ePULongLong,
                 eFloat = 72, eDouble, eLongDouble,
                 eImaginary = 80, eImaginaryDouble, eImaginaryLongDouble,
                 eComplex = 88, eComplexDouble, eComplexLongDouble,
                 ePFloat = 96, ePDouble, ePLongDouble,
                 ePImaginary = 104, ePImaginaryDouble, ePImaginaryLongDouble,
                 ePComplex = 112, ePComplexDouble, ePComplexLongDouble,
                 eReservedTop = 1023
    } ;
    ObjType(eType Type, ObjInt Index) : name(""), type(Type), base(0), top(1), 
                constVal(0), baseType(nullptr), index(Index),
                indexType(&defaultIndexType) {}
    ObjType(eType Type, ObjType *BaseType, ObjInt Index) : name(""), type(Type), base(0), top(1), 
                index(Index), constVal(0), baseType(BaseType), 
                indexType(&defaultIndexType) {}
    ObjType(ObjString Name, eType Type, ObjType *BaseType, ObjInt Index) 
                : name(Name), type(Type), base(0), top(1), index(Index),
                constVal(0), baseType(BaseType), 
                indexType(&defaultIndexType) {}
    virtual ~ObjType() {}
    ObjString &GetName() { return name; }
    void SetName(ObjString Name) {name = Name; }
    ObjInt GetIndex() { return index; }
    void SetIndex(ObjInt Index) { index = Index; }
    eType GetType() { return type; }
    void SetType(eType Type) { type = Type; }
    ObjInt GetBase() { return base; }
    void SetBase(ObjInt Base) { base = Base; }	
    ObjInt GetTop() { return top; }
    void SetTop(ObjInt Top) { top = Top; }	
    ObjInt GetConstVal() { return constVal; }
    void SetConstVal(ObjInt ConstVal) { constVal = ConstVal; }	
    ObjType *GetBaseType() { return baseType; }
    void SetBaseType(ObjType *BaseType) { baseType = BaseType; }
    ObjType *GetIndexType() { return indexType; }
    void SetIndexType(ObjType *IndexType) { indexType = IndexType; }
    void Add(ObjField *Field)
    {
        if (Field)
            fields.push_back(Field);
    }
    void SetSize(int sz) { size = sz; }
    int GetSize() { return size; }
    void SetStartBit(int sb) { startBit = sb; }
    int GetStartBit() { return startBit; }
    void SetBitCount(int bc) { bitCount = bc; }
    int GetBitCount() { return bitCount; }
    
    ObjInt GetFieldSize() { return fields.size(); }
    typedef FieldContainer::iterator FieldIterator;
    typedef FieldContainer::const_iterator const_FieldIterator;
    
    FieldIterator FieldBegin() { return fields.begin(); }
    FieldIterator FieldEnd() { return fields.end(); }

private:
    ObjString name;
    ObjInt index; /* index only makes sense for derived types */
    enum eType type;
    ObjType *baseType;
    ObjType *indexType;
    ObjInt base;
    ObjInt top;
    ObjInt constVal;
    int size;
    int startBit;
    int bitCount;
    FieldContainer fields;
    static ObjType defaultIndexType;
};
class ObjField: public ObjWrapper
{
public:
    ObjField(ObjString Name, ObjType *Base, ObjInt ConstVal, ObjInt index)
        : name(Name), base(Base), constVal(ConstVal), typeIndex(index) {}
        
    ObjString &GetName() { return name; }
    void SetName(ObjString Name) { name = Name; }
        
    ObjType *GetBase() { return base; }
    void SetBase(ObjType *Base) { base = Base; }
    
    ObjInt GetConstVal() { return constVal; }
    void SetConstVal(ObjInt ConstVal) { constVal = ConstVal; }

    ObjInt GetTypeIndex() { return typeIndex; }
    void SetTypeIndex(ObjInt index) { typeIndex = index; }
    
private:
    ObjString name;
    ObjType *base;
    ObjInt constVal;
    ObjInt typeIndex;
};

#endif