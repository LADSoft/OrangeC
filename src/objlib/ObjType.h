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

#ifndef OBJTYPE_H
#define OBJTYPE_H
#include <vector>
#include "ObjTypes.h"
#include "ObjIndexManager.h"

class ObjField;

class ObjType : public ObjWrapper
{
    typedef std::vector<ObjField*> FieldContainer;

  public:
    // clang-format off
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
    // clang-format on
    ObjType(eType Type, ObjInt Index) :
        name(""),
        type(Type),
        base(0),
        top(1),
        constVal(0),
        baseType(nullptr),
        index(Index),
        indexType(&defaultIndexType),
        size(0),
        startBit(0),
        bitCount(0)
    {
    }
    ObjType(eType Type, ObjType* BaseType, ObjInt Index) :
        name(""),
        type(Type),
        base(0),
        top(1),
        index(Index),
        constVal(0),
        baseType(BaseType),
        indexType(&defaultIndexType),
        size(0),
        startBit(0),
        bitCount(0)
    {
    }
    ObjType(const ObjString& Name, eType Type, ObjType* BaseType, ObjInt Index) :
        name(Name),
        type(Type),
        base(0),
        top(1),
        index(Index),
        constVal(0),
        baseType(BaseType),
        indexType(&defaultIndexType),
        size(0),
        startBit(0),
        bitCount(0)
    {
    }
    virtual ~ObjType() {}
    ObjString& GetName() { return name; }
    void SetName(ObjString& Name) { name = Name; }
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
    ObjType* GetBaseType() { return baseType; }
    void SetBaseType(ObjType* BaseType) { baseType = BaseType; }
    ObjType* GetIndexType() { return indexType; }
    void SetIndexType(ObjType* IndexType) { indexType = IndexType; }
    void Add(ObjField* Field)
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
    typedef FieldContainer::iterator iterator;
    typedef FieldContainer::const_iterator const_iterator;

    iterator begin() { return fields.begin(); }
    iterator end() { return fields.end(); }

  private:
    ObjString name;
    ObjInt index; /* index only makes sense for derived types */
    enum eType type;
    ObjType* baseType;
    ObjType* indexType;
    ObjInt base;
    ObjInt top;
    ObjInt constVal;
    int size;
    int startBit;
    int bitCount;
    FieldContainer fields;
    static ObjType defaultIndexType;
};
class ObjField : public ObjWrapper
{
  public:
    ObjField(const ObjString& Name, ObjType* Base, ObjInt ConstVal, ObjInt index) :
        name(Name), base(Base), constVal(ConstVal), typeIndex(index)
    {
    }

    ObjString& GetName() { return name; }
    void SetName(ObjString& Name) { name = Name; }

    ObjType* GetBase() { return base; }
    void SetBase(ObjType* Base) { base = Base; }

    ObjInt GetConstVal() { return constVal; }
    void SetConstVal(ObjInt ConstVal) { constVal = ConstVal; }

    ObjInt GetTypeIndex() { return typeIndex; }
    void SetTypeIndex(ObjInt index) { typeIndex = index; }

  private:
    ObjString name;
    ObjType* base;
    ObjInt constVal;
    ObjInt typeIndex;
};

#endif