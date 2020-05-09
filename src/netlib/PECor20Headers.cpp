/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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
 */

#include "DotNetPELib.h"
#include "PEFile.h"
#include "MZHeader.h"
#include "PEHeader.h"
#include <time.h>
#include <stdio.h>
namespace DotNetPELib
{
size_t IndexBase::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    size_t rv;
    DWord val = (index_ << GetIndexShift()) + tag_;
    if (HasIndexOverflow(sizes))
    {
        *(DWord*)dest = val;
        rv = 4;
    }
    else
    {
        *(Word*)dest = val;
        rv = 2;
    }
    return rv;
}
size_t IndexBase::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    size_t val, rv;
    if (HasIndexOverflow(sizes))
    {
        val = *(DWord*)src;
        rv = 4;
    }
    else
    {
        val = *(Word*)src;
        rv = 2;
    }
    index_ = val >> GetIndexShift();
    tag_ = val & ((1 << GetIndexShift()) - 1);
    return rv;
}
bool ResolutionScope::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
{
    return Large(sizes[tModule]) || Large(sizes[tModuleRef]) || Large(sizes[tAssemblyRef]) || Large(sizes[tTypeRef]);
}
bool TypeDefOrRef::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
{
    return Large(sizes[tTypeDef]) || Large(sizes[tTypeRef]) || Large(sizes[tTypeSpec]);
}
bool TypeOrMethodDef::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
{
    return Large(sizes[tTypeDef]) || Large(sizes[tMethodDef]);
}
bool MethodDefOrRef::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
{
    return Large(sizes[tMethodDef]) || Large(sizes[tMemberRef]);
}
bool MemberRefParent::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
{
    return Large(sizes[tTypeDef]) || Large(sizes[tTypeRef]) || Large(sizes[tModule]) || Large(sizes[tMethodDef]) ||
           Large(sizes[tTypeSpec]);
}
bool Constant::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
{
    return Large(sizes[tField]) || Large(sizes[tParam]);
}
bool CustomAttribute::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
{
    return Large(sizes[tMethodDef]) || Large(sizes[tField]) || Large(sizes[tTypeRef]) || Large(sizes[tTypeDef]) ||
           Large(sizes[tParam]) || Large(sizes[tImplMap]) || Large(sizes[tMemberRef]) || Large(sizes[tModule]) ||
           Large(sizes[tStandaloneSig]) || Large(sizes[tModuleRef]) || Large(sizes[tTypeSpec]) || Large(sizes[tAssemblyDef]) ||
           Large(sizes[tAssemblyRef]);
}
bool CustomAttributeType::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
{
    return Large(sizes[tMethodDef]) || Large(sizes[tMemberRef]);
}
bool MemberForwarded::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
{
    return Large(sizes[tField]) || Large(sizes[tMethodDef]);
}
bool EventList::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const { return Large(sizes[tEvent]); }
bool FieldList::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const { return Large(sizes[tField]); }
bool MethodList::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const { return Large(sizes[tMethodDef]); }
bool ParamList::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const { return Large(sizes[tParam]); }
bool PropertyList::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const { return Large(sizes[tProperty]); }
bool TypeDef::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const { return Large(sizes[tTypeDef]); }
bool ModuleRef::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const { return Large(sizes[tModuleRef]); }
bool DeclSecurity::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
{
    return Large(sizes[tMethodDef]) || Large(sizes[tTypeDef]) || Large(sizes[tMethodDef]) || Large(sizes[tAssemblyDef]);
}
bool Semantics::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
{
    return Large(sizes[tEvent]) || Large(sizes[tProperty]);
}
bool FieldMarshal::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
{
    return Large(sizes[tField]) || Large(sizes[tParam]);
}
bool GenericRef::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const { return Large(sizes[tGenericParam]); }
bool Implementation::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const
{
    return Large(sizes[tFile]) || Large(sizes[tAssemblyRef]);
}
bool String::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const { return Large(sizes[tString]); }
bool US::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const { return Large(sizes[tUS]); }
bool GUID::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const { return Large(sizes[tGUID]); }
bool Blob::HasIndexOverflow(size_t sizes[MaxTables + ExtraIndexes]) const { return Large(sizes[tBlob]); }
size_t ModuleTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    *(Word*)dest = 0;
    size_t n = 2;
    n += nameIndex_.Render(sizes, dest + n);
    n += guidIndex_.Render(sizes, dest + n);
    if (sizes[tGUID] > 65535)
    {
        *(DWord*)(dest + n) = 0;
        n += 4;
        *(DWord*)(dest + n) = 0;
        n += 4;
    }
    else
    {
        *(Word*)(dest + n) = 0;
        n += 2;
        *(Word*)(dest + n) = 0;
        n += 2;
    }
    return n;
}
size_t ModuleTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    int n = 2;
    n += nameIndex_.Get(sizes, src + n);
    n += guidIndex_.Get(sizes, src + n);
    if (sizes[tGUID] > 65535)
    {
        n += 8;
    }
    else
    {
        n += 4;
    }
    return n;
}
size_t TypeRefTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    int n = resolution_.Render(sizes, dest);
    n += typeNameIndex_.Render(sizes, dest + n);
    n += typeNameSpaceIndex_.Render(sizes, dest + n);
    return n;
}
size_t TypeRefTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    int n = resolution_.Get(sizes, src);
    n += typeNameIndex_.Get(sizes, src + n);
    n += typeNameSpaceIndex_.Get(sizes, src + n);
    return n;
}
size_t TypeDefTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    *(DWord*)dest = flags_;
    int n = 4;
    n += typeNameIndex_.Render(sizes, dest + n);
    n += typeNameSpaceIndex_.Render(sizes, dest + n);
    n += extends_.Render(sizes, dest + n);
    n += fields_.Render(sizes, dest + n);
    n += methods_.Render(sizes, dest + n);
    return n;
}
size_t TypeDefTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    flags_ = *(DWord*)src;
    int n = 4;
    n += typeNameIndex_.Get(sizes, src + n);
    n += typeNameSpaceIndex_.Get(sizes, src + n);
    n += extends_.Get(sizes, src + n);
    n += fields_.Get(sizes, src + n);
    n += methods_.Get(sizes, src + n);
    return n;
}
size_t FieldTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    *(Word*)dest = flags_;
    int n = 2;
    n += nameIndex_.Render(sizes, dest + n);
    n += signatureIndex_.Render(sizes, dest + n);
    return n;
}
size_t FieldTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    flags_ = *(Word*)src;
    int n = 2;
    n += nameIndex_.Get(sizes, src + n);
    n += signatureIndex_.Get(sizes, src + n);
    return n;
}
size_t MethodDefTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    *(DWord*)dest = method_->rva_;
    int n = 4;
    *(Word*)(dest + n) = implFlags_;
    n += 2;
    *(Word*)(dest + n) = flags_;
    n += 2;
    n += nameIndex_.Render(sizes, dest + n);
    n += signatureIndex_.Render(sizes, dest + n);
    n += paramIndex_.Render(sizes, dest + n);
    return n;
}
size_t MethodDefTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    rva_ = *(DWord*)src;
    int n = 4;
    implFlags_ = *(Word*)(src + n);
    n += 2;
    flags_ = *(Word*)(src + n);
    n += 2;
    n += nameIndex_.Get(sizes, src + n);
    n += signatureIndex_.Get(sizes, src + n);
    n += paramIndex_.Get(sizes, src + n);
    return n;
}
size_t ParamTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    *(Word*)dest = flags_;
    int n = 2;
    *(Word*)(dest + n) = sequenceIndex_;
    n += 2;
    n += nameIndex_.Render(sizes, dest + n);

    return n;
}
size_t ParamTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    flags_ = *(Word*)src;
    int n = 2;
    sequenceIndex_ = *(Word*)(src + n);
    n += 2;
    n += nameIndex_.Get(sizes, src + n);
    return n;
}
size_t InterfaceImplTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    int n = class_.Render(sizes, dest);
    n += interface_.Render(sizes, dest + n);
    return n;
}
size_t InterfaceImplTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    int n = class_.Get(sizes, src);
    n += interface_.Get(sizes, src + n);
    return n;
}
size_t MemberRefTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    int n = parentIndex_.Render(sizes, dest);
    n += nameIndex_.Render(sizes, dest + n);
    n += signatureIndex_.Render(sizes, dest + n);
    return n;
}
size_t MemberRefTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    int n = parentIndex_.Get(sizes, src);
    n += nameIndex_.Get(sizes, src + n);
    n += signatureIndex_.Get(sizes, src + n);
    return n;
}
size_t ConstantTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    *(Byte*)dest = type_;
    *(Byte*)(dest + 1) = 0;
    int n = 2;
    n += parentIndex_.Render(sizes, dest + n);
    n += valueIndex_.Render(sizes, dest + n);
    return n;
}
size_t ConstantTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    type_ = *(Byte*)src;
    int n = 2;
    n += parentIndex_.Get(sizes, src + n);
    n += valueIndex_.Get(sizes, src + n);
    return n;
}
size_t CustomAttributeTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    int n = parentIndex_.Render(sizes, dest);
    n += typeIndex_.Render(sizes, dest + n);
    n += valueIndex_.Render(sizes, dest + n);
    return n;
}
size_t CustomAttributeTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    int n = parentIndex_.Get(sizes, src);
    n += typeIndex_.Get(sizes, src + n);
    n += valueIndex_.Get(sizes, src + n);
    return n;
}
size_t FieldMarshalTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    int n = parent_.Render(sizes, dest);
    n += nativeType_.Render(sizes, dest + n);
    return n;
}
size_t FieldMarshalTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    int n = parent_.Get(sizes, src);
    n += nativeType_.Get(sizes, src + n);
    return n;
}
size_t DeclSecurityTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    *(Word*)dest = action_;
    int n = 2;
    n += parent_.Render(sizes, dest + n);
    n += permissionSet_.Render(sizes, dest + n);
    return n;
}
size_t DeclSecurityTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    action_ = *(Word*)src;
    int n = 2;
    n += parent_.Get(sizes, src + n);
    n += permissionSet_.Get(sizes, src + n);
    return n;
}
size_t ClassLayoutTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    *(Word*)dest = pack_;
    int n = 2;
    *(DWord*)(dest + n) = size_;
    n += 4;
    n += parent_.Render(sizes, dest + n);
    return n;
}
size_t ClassLayoutTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    pack_ = *(Word*)src;
    int n = 2;
    size_ = *(DWord*)(src + n);
    n += 4;
    n += parent_.Get(sizes, src + n);
    return n;
}
size_t FieldLayoutTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    *(DWord*)dest = offset_;
    int n = 4;
    n += parent_.Render(sizes, dest + n);
    return n;
}
size_t FieldLayoutTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    offset_ = *(DWord*)src;
    int n = 4;
    n += parent_.Get(sizes, src + n);
    return n;
}
size_t StandaloneSigTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    return signatureIndex_.Render(sizes, dest);
}
size_t StandaloneSigTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src) { return signatureIndex_.Get(sizes, src); }
size_t EventMapTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    int n = parent_.Render(sizes, dest);
    n += eventList_.Render(sizes, dest + n);
    return n;
}
size_t EventMapTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    int n = parent_.Get(sizes, src);
    n += eventList_.Get(sizes, src + n);
    return n;
}
size_t EventTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    *(Word*)dest = flags_;
    int n = 2;
    n += name_.Render(sizes, dest + n);
    n += eventType_.Render(sizes, dest + n);
    return n;
}
size_t EventTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    flags_ = *(Word*)src;
    int n = 2;
    n += name_.Get(sizes, src + n);
    n += eventType_.Get(sizes, src + n);
    return n;
}
size_t PropertyMapTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    int n = parent_.Render(sizes, dest);
    n += propertyList_.Render(sizes, dest + n);
    return n;
}
size_t PropertyMapTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    int n = parent_.Get(sizes, src);
    n += propertyList_.Get(sizes, src + n);
    return n;
}
size_t PropertyTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    *(Word*)dest = flags_;
    int n = 2;
    n += name_.Render(sizes, dest + n);
    n += propertyType_.Render(sizes, dest + n);
    return n;
}
size_t PropertyTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    flags_ = *(Word*)src;
    int n = 2;
    n += name_.Get(sizes, src + n);
    n += propertyType_.Get(sizes, src + n);
    return n;
}
size_t MethodSemanticsTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    *(Word*)dest = semantics_;
    int n = 2;
    n += method_.Render(sizes, dest + n);
    n += association_.Render(sizes, dest + n);
    return n;
}
size_t MethodSemanticsTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    semantics_ = *(Word*)src;
    int n = 2;
    n += method_.Get(sizes, src + n);
    n += association_.Get(sizes, src + n);
    return n;
}
size_t MethodImplTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    int n = class_.Render(sizes, dest);
    n += methodBody_.Render(sizes, dest + n);
    n += methodDeclaration_.Render(sizes, dest + n);
    return n;
}
size_t MethodImplTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    int n = class_.Get(sizes, src);
    n += methodBody_.Get(sizes, src + n);
    n += methodDeclaration_.Get(sizes, src + n);
    return n;
}
size_t ModuleRefTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    return nameIndex_.Render(sizes, dest);
}
size_t ModuleRefTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src) { return nameIndex_.Get(sizes, src); }
size_t TypeSpecTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    return signatureIndex_.Render(sizes, dest);
}
size_t TypeSpecTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src) { return signatureIndex_.Get(sizes, src); }
size_t ImplMapTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    *(Word*)dest = flags_;
    int n = 2;
    n += methodIndex_.Render(sizes, dest + n);
    n += importNameIndex_.Render(sizes, dest + n);
    n += moduleIndex_.Render(sizes, dest + n);
    return n;
}
size_t ImplMapTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    flags_ = *(Word*)src;
    int n = 2;
    n += methodIndex_.Get(sizes, src + n);
    n += importNameIndex_.Get(sizes, src + n);
    n += moduleIndex_.Get(sizes, src + n);
    return n;
}
size_t FieldRVATableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    // note that when reading rva_ holds the rva, when writing it holds an offset into the CIL section
    *(DWord*)dest = rva_ + PEWriter::cildata_rva_;
    int n = 4;
    n += fieldIndex_.Render(sizes, dest + n);
    return n;
}
size_t FieldRVATableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    // note that when reading rva_ holds the rva, when writing it holds an offset into the CIL section
    rva_ = *(DWord*)src;
    int n = 4;
    n += fieldIndex_.Get(sizes, src + n);
    return n;
}
size_t AssemblyDefTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    *(DWord*)dest = DefaultHashAlgId;
    int n = 4;
    // assembly version
    *(Word*)(dest + n) = major_;
    n += 2;
    *(Word*)(dest + n) = minor_;
    n += 2;
    *(Word*)(dest + n) = build_;
    n += 2;
    *(Word*)(dest + n) = revision_;
    n += 2;
    *(DWord*)(dest + n) = flags_;
    n += 4;
    n += publicKeyIndex_.Render(sizes, dest + n);
    n += nameIndex_.Render(sizes, dest + n);
    n += cultureIndex_.Render(sizes, dest + n);
    return n;
}
size_t AssemblyDefTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    // always assume the right hash algorithm as there is currently only one spec'd
    int n = 4;
    // assembly version
    major_ = *(Word*)(src + n);
    n += 2;
    minor_ = *(Word*)(src + n);
    n += 2;
    build_ = *(Word*)(src + n);
    n += 2;
    revision_ = *(Word*)(src + n);
    n += 2;
    flags_ = *(DWord*)(src + n);
    n += 4;
    n += publicKeyIndex_.Get(sizes, src + n);
    n += nameIndex_.Get(sizes, src + n);
    n += cultureIndex_.Get(sizes, src + n);
    return n;
}
size_t AssemblyRefTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    int n = 0;
    // assembly version
    *(Word*)(dest + n) = major_;
    n += 2;
    *(Word*)(dest + n) = minor_;
    n += 2;
    *(Word*)(dest + n) = build_;
    n += 2;
    *(Word*)(dest + n) = revision_;
    n += 2;
    *(DWord*)(dest + n) = flags_;
    n += 4;
    n += publicKeyIndex_.Render(sizes, dest + n);
    n += nameIndex_.Render(sizes, dest + n);
    n += cultureIndex_.Render(sizes, dest + n);
    n += hashIndex_.Render(sizes, dest + n);
    return n;
}
size_t AssemblyRefTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    int n = 0;
    // assembly version
    major_ = *(Word*)(src + n);
    n += 2;
    minor_ = *(Word*)(src + n);
    n += 2;
    build_ = *(Word*)(src + n);
    n += 2;
    revision_ = *(Word*)(src + n);
    n += 2;
    flags_ = *(DWord*)(src + n);
    n += 4;
    n += publicKeyIndex_.Get(sizes, src + n);
    n += nameIndex_.Get(sizes, src + n);
    n += cultureIndex_.Get(sizes, src + n);
    n += hashIndex_.Get(sizes, src + n);
    return n;
}
size_t FileTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    *(DWord*)dest = flags_;
    int n = 4;
    n += name_.Render(sizes, dest + n);
    n += hash_.Render(sizes, dest + n);
    return n;
}
size_t FileTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    flags_ = *(DWord*)src;
    int n = 4;
    n += name_.Get(sizes, src + n);
    n += hash_.Get(sizes, src + n);
    return n;
}
size_t ExportedTypeTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    *(DWord*)(dest) = flags_;
    int n = 4;
    n += typeDefId_.Render(sizes, dest + n);
    n += typeName_.Render(sizes, dest + n);
    n += typeNameSpace_.Render(sizes, dest + n);
    n += implementation_.Render(sizes, dest + n);
    return n;
}
size_t ExportedTypeTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    flags_ = *(DWord*)(src);
    int n = 4;
    n += typeDefId_.Get(sizes, src + n);
    n += typeName_.Get(sizes, src + n);
    n += typeNameSpace_.Get(sizes, src + n);
    n += implementation_.Get(sizes, src + n);
    return n;
}
size_t ManifestResourceTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    *(DWord*)dest = offset_;
    *(DWord*)(dest + 4) = flags_;
    int n = 8;
    n += name_.Render(sizes, dest + n);
    n += implementation_.Render(sizes, dest + n);
    return n;
}
size_t ManifestResourceTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    offset_ = *(DWord*)src;
    flags_ = *(DWord*)(src + 4);
    int n = 8;
    n += name_.Get(sizes, src + n);
    n += implementation_.Get(sizes, src + n);
    return n;
}
size_t NestedClassTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    int n = nestedIndex_.Render(sizes, dest);
    n += enclosingIndex_.Render(sizes, dest + n);
    return n;
}
size_t NestedClassTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    int n = nestedIndex_.Get(sizes, src);
    n += enclosingIndex_.Get(sizes, src + n);
    return n;
}
size_t GenericParamTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    *(Word*)dest = number_;
    *(Word*)(dest + 2) = flags_;
    int n = 4;
    n += owner_.Render(sizes, dest + n);
    n += name_.Render(sizes, dest + n);
    return n;
}
size_t GenericParamTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    number_ = *(Word*)src;
    flags_ = *(Word*)(src + 2);
    int n = 4;
    n += owner_.Get(sizes, src + n);
    n += name_.Get(sizes, src + n);
    return n;
}
size_t MethodSpecTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    int n = method_.Render(sizes, dest);
    n += instantiation_.Render(sizes, dest + n);
    return n;
}
size_t MethodSpecTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    int n = method_.Get(sizes, src);
    n += instantiation_.Get(sizes, src + n);
    return n;
}
size_t GenericParamConstraintsTableEntry::Render(size_t sizes[MaxTables + ExtraIndexes], Byte* dest) const
{
    int n = owner_.Render(sizes, dest);
    n += constraint_.Render(sizes, dest + n);
    return n;
}
size_t GenericParamConstraintsTableEntry::Get(size_t sizes[MaxTables + ExtraIndexes], Byte* src)
{
    int n = owner_.Get(sizes, src);
    n += constraint_.Get(sizes, src + n);
    return n;
}
}  // namespace DotNetPELib