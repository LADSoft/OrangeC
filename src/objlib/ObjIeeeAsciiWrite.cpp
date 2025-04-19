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

#include "ObjFactory.h"
#include "ObjIeee.h"
#include <cstdio>
#include <deque>

char ObjIeeeAscii::lineend[2] = {10};
template <typename T>
inline constexpr int min(T x, T y)
{
    return x < y ? x : y;
}
void ObjIeeeAscii::bufferup(const char* data, int len)
{
    if (len + ioBufferLen > BUFFERSIZE)
    {
        flush();
    }
    if (len + ioBufferLen > BUFFERSIZE)
    {
        fwrite(data, len, 1, sfile);
    }
    else
    {
        memcpy(ioBuffer.get() + ioBufferLen, data, len);
        ioBufferLen += len;
    }
}
ObjString ObjIeeeAscii::GetSymbolName(ObjSymbol* Symbol)
{
    ObjString name;
    switch (Symbol->GetType())
    {
        case ObjSymbol::ePublic:
        default:
            name = "I";
            break;
        case ObjSymbol::eExternal:
            name = "X";
            break;
        case ObjSymbol::eLocal:
            name = "N";
            break;
        case ObjSymbol::eAuto:
            name = "A";
            break;
        case ObjSymbol::eReg:
            name = "E";
            break;
    }
    name = name + ObjUtil::ToHex(Symbol->GetIndex());
    return name;
}
ObjString ObjIeeeAscii::ToString(const ObjString& strng) { return ObjUtil::ToHex(strng.length(), 3) + strng; }
ObjString ObjIeeeAscii::ToTime(std::tm tms)
{
    ObjString rv;
    rv = ObjUtil::ToDecimal(tms.tm_year + 1900, 4) + ObjUtil::ToDecimal(tms.tm_mon + 1, 2) + ObjUtil::ToDecimal(tms.tm_mday, 2) +
         ObjUtil::ToDecimal(tms.tm_hour, 2) + ObjUtil::ToDecimal(tms.tm_min, 2) + ObjUtil::ToDecimal(tms.tm_sec, 2);
    return rv;
}
void ObjIeeeAscii::RenderFile(ObjSourceFile* File)
{
    ObjString data(ObjUtil::ToDecimal(File->GetIndex()) + "," + ToString(File->GetName()) + "," + ToTime(File->GetFileTime()));
    RenderComment(eSourceFile, std::move(data));
}
ObjString ObjIeeeAscii::GetTypeIndex(ObjType* Type)
{
    if (Type->GetType() < ObjType::eVoid)
        return ObjUtil::ToHex(Type->GetIndex());
    return ObjUtil::ToHex((int)Type->GetType());
}
void ObjIeeeAscii::RenderStructure(ObjType* Type)
{
    const int MaxPerLine = 15;
    std::deque<ObjField*> fields;
    for (auto&& field : *Type)
    {
        fields.push_front(field);
    }
    ObjString lastIndex;
    while (!fields.empty())
    {
        int bottom;
        ObjString index;
        ObjString baseType;
        ObjField* current = fields.front();
        for (bottom = 1; bottom < fields.size(); bottom++)
        {
            if (fields[bottom]->GetTypeIndex() != current->GetTypeIndex())
                break;
        }
        // the problem with this is if they output the file twice
        // the types won't match
        if (fields.size() - bottom == 0)
        {
            index = ObjUtil::ToHex(Type->GetIndex());
            baseType = ObjUtil::ToHex(Type->GetType());
            RenderString("ATT" + index + ",T" + baseType + "," + ObjUtil::ToHex(Type->GetSize()));
        }
        else
        {
            index = ObjUtil::ToHex(current->GetTypeIndex());
            baseType = ObjUtil::ToHex(ObjType::eField);
            RenderString("ATT" + index + ",T" + baseType);
        }
        for (unsigned j = 0; j < bottom; j++)
        {
            ObjField* currentField = fields[bottom - j - 1];
            RenderString(",T" + GetTypeIndex(currentField->GetBase()) + ",");
            RenderString(ToString(currentField->GetName()));
            RenderString("," + ObjUtil::ToHex(currentField->GetConstVal()));
        }
        RenderString(lastIndex + ".");
        endl();
        lastIndex = ",T" + index;
        for (unsigned j = 0; j < bottom; j++)
            fields.pop_front();
    }
}
void ObjIeeeAscii::RenderFunction(ObjFunction* Function)
{
    RenderString("ATT" + GetTypeIndex(static_cast<ObjType*>(Function)) + ",");
    RenderString("T" + ObjUtil::ToHex(ObjType::eFunction) + ",");
    RenderString("T" + GetTypeIndex(Function->GetReturnType()) + ",");
    RenderString(ObjUtil::ToHex(Function->GetLinkage()));
    // assuming a reasonable number of parameters
    // parameters are TYPES
    for (auto func : *Function)
    {
        RenderString(",T" + GetTypeIndex(func));
    }
    RenderCstr(".");
    endl();
}
void ObjIeeeAscii::RenderType(ObjType* Type)
{
    switch (Type->GetType())
    {
        case ObjType::ePointer:
        case ObjType::eLRef:
        case ObjType::eRRef:
            RenderString("ATT" + GetTypeIndex(Type));
            RenderString(",T" + ObjUtil::ToHex(Type->GetType()));
            RenderString("," + ObjUtil::ToHex(Type->GetSize()));
            RenderString(",T" + GetTypeIndex(Type->GetBaseType()) + ".");
            endl();
            break;
        case ObjType::eTypeDef:
            RenderString("ATT" + GetTypeIndex(Type));
            RenderString(",T" + ObjUtil::ToHex(ObjType::eTypeDef));
            RenderString(",T" + GetTypeIndex(Type->GetBaseType()) + ".");
            endl();
            break;
        case ObjType::eFunction:
            RenderFunction(static_cast<ObjFunction*>(Type));
            break;
        case ObjType::eStruct:
        case ObjType::eUnion:
        case ObjType::eEnum:
            RenderStructure(Type);
            break;
        case ObjType::eBitField:
            RenderString("ATT" + GetTypeIndex(Type));
            RenderString(",T" + ObjUtil::ToHex((int)ObjType::eBitField));
            RenderString("," + ObjUtil::ToHex(Type->GetSize()));
            RenderString(",T" + GetTypeIndex(Type->GetBaseType()));
            RenderString("," + ObjUtil::ToHex(Type->GetStartBit()));
            RenderString("," + ObjUtil::ToHex(Type->GetBitCount()) + ".");
            endl();
            break;
        case ObjType::eArray:
            RenderString("ATT" + GetTypeIndex(Type));
            RenderString(",T" + ObjUtil::ToHex((int)Type->GetType()));
            RenderString("," + ObjUtil::ToHex(Type->GetSize()));
            RenderString(",T" + GetTypeIndex(Type->GetBaseType()));
            RenderString(",T" + GetTypeIndex(Type->GetIndexType()));
            RenderString("," + ObjUtil::ToHex(Type->GetBase()));
            RenderString("," + ObjUtil::ToHex(Type->GetTop()) + ".");
            endl();
            break;
        case ObjType::eVla:
            RenderString("ATT" + GetTypeIndex(Type));
            RenderString(",T" + ObjUtil::ToHex((int)Type->GetType()));
            RenderString("," + ObjUtil::ToHex(Type->GetSize()));
            RenderString(",T" + GetTypeIndex(Type->GetBaseType()));
            RenderString(",T" + GetTypeIndex(Type->GetIndexType()));
            endl();
            break;
        default:
            break;
    }
    if (Type->GetType() < ObjType::eVoid && !Type->GetName().empty())
    {
        RenderString("NT" + ObjUtil::ToHex(Type->GetIndex()));
        RenderString("," + ToString(Type->GetName()) + ".");
        endl();
    }
}
void ObjIeeeAscii::RenderSymbol(ObjSymbol* Symbol)
{
    if (Symbol->GetType() == ObjSymbol::eDefinition)
    {
        ObjDefinitionSymbol* dsym = static_cast<ObjDefinitionSymbol*>(Symbol);
        ObjString data = ToString(dsym->GetName()) + "," + ObjUtil::ToDecimal(dsym->GetValue());
        RenderComment(eDefinition,std::move(data));
    }
    else if (Symbol->GetType() == ObjSymbol::eImport)
    {
        ObjString data;
        ObjImportSymbol* isym = static_cast<ObjImportSymbol*>(Symbol);
        if (isym->GetByOrdinal())
            data = "O," + ToString(isym->GetName()) + "," + ObjUtil::ToDecimal(isym->GetOrdinal()) + "," +
                   ToString(isym->GetDllName());
        else
            data = "N," + ToString(isym->GetName()) + "," + ToString(isym->GetExternalName()) + "," + ToString(isym->GetDllName());
        RenderComment(eImport, std::move(data));
    }
    else if (Symbol->GetType() == ObjSymbol::eExport)
    {
        ObjString data;
        ObjExportSymbol* esym = static_cast<ObjExportSymbol*>(Symbol);
        if (esym->GetByOrdinal())
            data = "O," + ToString(esym->GetName()) + "," + ObjUtil::ToDecimal(esym->GetOrdinal());
        else
            data = "N," + ToString(esym->GetName()) + "," + ToString(esym->GetExternalName());
        if (!esym->GetDllName().empty())
            data = data + "," + esym->GetDllName();
        RenderComment(eExport, std::move(data));
    }
    else if (Symbol->GetType() != ObjSymbol::eLabel)
    {
        ObjString name = GetSymbolName(Symbol);
        RenderString("N" + name + "," + ToString(Symbol->GetName()) + ".");
        endl();
        if (Symbol->GetOffset())
        {
            RenderString("AS" + name + ",");
            RenderExpression(Symbol->GetOffset());
            RenderCstr(".");
            endl();
        }
        if (GetDebugInfoFlag() && Symbol->GetBaseType())
        {
            RenderString("AT" + name + ",T" + GetTypeIndex(Symbol->GetBaseType()) + ".");
            endl();
        }
    }
}
void ObjIeeeAscii::RenderSection(ObjSection* Section)
{
    // This is actually the section header information
    RenderString("ST" + ObjUtil::ToHex(Section->GetIndex()) + ",");
    ObjInt quals = Section->GetQuals();
    if (quals & ObjSection::absolute)
        RenderCstr("A,");
    if (quals & ObjSection::bit)
        RenderCstr("B,");
    if (quals & ObjSection::common)
        RenderCstr("C,");
    if (quals & ObjSection::equal)
        RenderCstr("E,");
    if (quals & ObjSection::max)
        RenderCstr("M,");
    if (quals & ObjSection::now)
        RenderCstr("N,");
    if (quals & ObjSection::postpone)
        RenderCstr("P,");
    if (quals & ObjSection::rom)
        RenderCstr("R,");
    if (quals & ObjSection::separate)
        RenderCstr("S,");
    if (quals & ObjSection::unique)
        RenderCstr("U,");
    if (quals & ObjSection::ram)
        RenderCstr("W,");
    if (quals & ObjSection::exec)
        RenderCstr("X,");
    if (quals & ObjSection::zero)
        RenderCstr("Z,");
    if (quals & ObjSection::virt)
        RenderCstr("V,");

    // this assums a section number < 160... otherwise it could be an attrib
    RenderString(ToString(Section->GetName()) + ".");
    endl();

    RenderString("SA" + ObjUtil::ToHex(Section->GetIndex()) + "," + ObjUtil::ToHex(Section->GetAlignment()) + ".");
    endl();
    RenderString("ASS" + ObjUtil::ToHex(Section->GetIndex()) + "," + ObjUtil::ToHex(Section->GetMemoryManager().GetSize()) + ".");
    endl();
    if (Section->GetVirtualType())
    {
        int n = Section->GetVirtualType()->GetIndex();
        if (n < ObjType::eReservedTop + 1)
            n = Section->GetVirtualType()->GetType();
        RenderString("ATR" + ObjUtil::ToHex(Section->GetIndex()) + ",T" + ObjUtil::ToHex(n) + ".");
        endl();
    }
    if (quals & ObjSection::absolute)
    {
        RenderString("ASL" + ObjUtil::ToHex(Section->GetIndex()) + "," + ObjUtil::ToHex(Section->GetMemoryManager().GetBase()) +
                     ".");
        endl();
    }
}
void ObjIeeeAscii::RenderDebugTag(ObjDebugTag* Tag)
{
    ObjString data;
    switch (Tag->GetType())
    {
        case ObjDebugTag::eVar:
            /* debugger has to dereference the name */
            if (!Tag->GetSymbol()->IsSectionRelative() && Tag->GetSymbol()->GetType() != ObjSymbol::eExternal)
            {
                data = GetSymbolName(Tag->GetSymbol());
                RenderComment(eVar, std::move(data));
            }
            break;
        case ObjDebugTag::eBlockStart:
        case ObjDebugTag::eBlockEnd:
            RenderComment(Tag->GetType() == ObjDebugTag::eBlockStart ? eBlockStart : eBlockEnd, std::move(ObjString("")));
            break;
        case ObjDebugTag::eFunctionStart:
        case ObjDebugTag::eFunctionEnd:
            data = GetSymbolName(Tag->GetSymbol());
            RenderComment(Tag->GetType() == ObjDebugTag::eFunctionStart ? eFunctionStart : eFunctionEnd, std::move(data));
            break;
        case ObjDebugTag::eVirtualFunctionStart:
        case ObjDebugTag::eVirtualFunctionEnd:
            data = "R" + ObjUtil::ToHex(Tag->GetSection()->GetIndex());
            RenderComment(Tag->GetType() == ObjDebugTag::eVirtualFunctionStart ? eFunctionStart : eFunctionEnd, std::move(data));
            break;
        case ObjDebugTag::eLineNo:
            data = ObjUtil::ToDecimal(Tag->GetLineNo()->GetFile()->GetIndex()) + "," +
                   ObjUtil::ToDecimal(Tag->GetLineNo()->GetLineNumber());
            RenderComment(eLineNo, std::move(data));
            break;
        default:
            break;
    }
}
void ObjIeeeAscii::RenderMemory(ObjMemoryManager* Memory)
{
    // this function is optimized to not use C++ stream objects
    // because it is called a lot, and the resultant memory allocations
    // really slow down linker and librarian operations
    char scratch[256];
    int n;
    scratch[0] = 'L';
    scratch[1] = 'D';
    n = 2;
    for (auto memory : *Memory)
    {
        if ((memory->HasDebugTags() && GetDebugInfoFlag()) || memory->GetFixup())
        {
            if (n != 2)
            {
                scratch[n++] = '.';
                scratch[n++] = 0;
                RenderCstr(scratch);
                endl();
                n = 2;
            }
            if (GetDebugInfoFlag() && memory->HasDebugTags())
            {
                for (auto* tag : *memory)
                {
                    RenderDebugTag(tag);
                }
            }
            if (memory->GetFixup())
            {
                RenderCstr("LR(");
                RenderExpression(memory->GetFixup());
                RenderString("," + ObjUtil::ToHex(memory->GetSize()) + ").");
                endl();
            }
        }
        if (memory->IsEnumerated())
        {
            if (n != 2)
            {
                scratch[n++] = '.';
                scratch[n++] = 0;
                RenderCstr(scratch);
                endl();
                n = 2;
            }
            RenderCstr("LE(");
            RenderString(ObjUtil::ToHex(memory->GetSize()));
            RenderString("," + ObjUtil::ToHex(memory->GetFill()) + ").");
            endl();
        }
        else if (memory->GetData())
        {
            ObjByte* p = memory->GetData();
            for (int i = 0; i < memory->GetSize(); i++)
            {
                int m = *p >> 4;
                if (m > 9)
                    m += 7;
                m += '0';
                scratch[n++] = m;
                m = *p++ & 0xf;
                if (m > 9)
                    m += 7;
                m += '0';
                scratch[n++] = m;
                if (n >= 66)
                {
                    scratch[n++] = '.';
                    scratch[n++] = 0;
                    RenderCstr(scratch);
                    endl();
                    n = 2;
                }
            }
        }
    }
    if (n != 2)
    {
        scratch[n++] = '.';
        scratch[n++] = 0;
        RenderCstr(scratch);
        endl();
    }
}
void ObjIeeeAscii::RenderMemoryBinary(ObjMemoryManager* Memory)
{
    char scratch[256];
    for (auto memory : *Memory)
    {
        if (memory->GetFixup())
        {
            *(unsigned*)scratch = memory->GetFixup()->Eval(0);
            bufferup(scratch, memory->GetSize());
        }
        if (memory->IsEnumerated())
        {
            memset(scratch, memory->GetFill(), sizeof(scratch));
            int len = memory->GetSize();
            while (len > sizeof(scratch))
            {
                bufferup(scratch, sizeof(scratch));
                len -= sizeof(scratch);
            }
            bufferup(scratch, len);
        }
        else if (memory->GetData())
        {
            bufferup((char*)memory->GetData(), memory->GetSize());
        }
    }
}
void ObjIeeeAscii::RenderBrowseInfo(ObjBrowseInfo* BrowseInfo)
{
    ObjString data;
    data = ObjUtil::ToHex((int)BrowseInfo->GetType()) + "," + ObjUtil::ToHex((int)BrowseInfo->GetQual()) + "," +
           ObjUtil::ToDecimal(BrowseInfo->GetLineNo()->GetFile()->GetIndex()) + "," +
           ObjUtil::ToDecimal(BrowseInfo->GetLineNo()->GetLineNumber()) + "," + ObjUtil::ToDecimal(BrowseInfo->GetCharPos()) + "," +
           ToString(BrowseInfo->GetData());
    RenderComment(eBrowseInfo, std::move(data));
}
void ObjIeeeAscii::RenderExpression(ObjExpression* Expression)
{
    switch (Expression->GetOp())
    {
        case ObjExpression::eNop:
            break;
        case ObjExpression::eNonExpression:
            RenderExpression(Expression->GetLeft());
            RenderCstr(",");
            RenderExpression(Expression->GetRight());
            break;
        case ObjExpression::eValue:
            RenderString(ObjUtil::ToHex(Expression->GetValue()));
            break;
        case ObjExpression::eAdd:
            RenderExpression(Expression->GetLeft());
            if (Expression->GetRight()->GetOp() != ObjExpression::eValue || Expression->GetRight()->GetValue() != 0)
            {
                RenderCstr(",");
                RenderExpression(Expression->GetRight());
                RenderCstr(",+");
            }
            break;
        case ObjExpression::eSub:
            RenderExpression(Expression->GetLeft());
            RenderCstr(",");
            RenderExpression(Expression->GetRight());
            RenderCstr(",-");
            break;
        case ObjExpression::eMul:
            RenderExpression(Expression->GetLeft());
            RenderCstr(",");
            RenderExpression(Expression->GetRight());
            RenderCstr(",*");
            break;
        case ObjExpression::eDiv:
            RenderExpression(Expression->GetLeft());
            RenderCstr(",");
            RenderExpression(Expression->GetRight());
            RenderCstr(",/");
            break;
        case ObjExpression::eExpression:
            RenderExpression(Expression->GetLeft());
            break;
        case ObjExpression::eSymbol:
            if (Expression->GetSymbol()->GetType() == ObjSymbol::eExternal)
            {
                // externals get embedded in the expression
                RenderString("X" + ObjUtil::ToHex(Expression->GetSymbol()->GetIndex()));
            }
            else
            {
                // other types of symbols we don't embed in the expression,
                // instead we embed their values
                RenderExpression(Expression->GetSymbol()->GetOffset());
            }
            break;
        case ObjExpression::eSection:
            RenderString("R" + ObjUtil::ToHex(Expression->GetSection()->GetIndex()));
            break;
        case ObjExpression::ePC:
            RenderCstr("P");
            break;
        default:
            break;
    }
}
void ObjIeeeAscii::RenderComment(eCommentType Type, ObjString strng)
{
    RenderString("CO" + ObjUtil::ToDecimal((int)Type, 3) + "," + ToString(strng) + ".");
    endl();
}
void ObjIeeeAscii::GatherCS(const char* Cstr)
{
    for (const char* data = Cstr; *data; data++)
    {
        if (*data >= ' ')
            cs += *data;
    }
}
bool ObjIeeeAscii::HandleWrite()
{
    ioBufferLen = 0;
    ioBuffer = std::make_unique<char[]>(BUFFERSIZE);
    ResetCS();
    WriteHeader();
    RenderCS();
    ResetCS();
    WriteFiles();
    RenderComment(eMakePass, std::move(ObjString("Make Pass Separator")));
    RenderCS();
    ResetCS();
    WriteTypes();
    RenderCS();
    ResetCS();
    WriteSectionHeaders();
    RenderCS();
    ResetCS();
    WriteSymbols();
    WriteStartAddress();
    RenderCS();
    ResetCS();
    RenderComment(eLinkPass,std::move( ObjString("Link Pass Separator")));
    WriteSections();
    RenderCS();
    ResetCS();
    RenderComment(eBrowsePass, std::move(ObjString("Browse Pass Separator")));
    WriteBrowseInfo();
    RenderCS();
    ResetCS();
    WriteTrailer();
    flush();
    ioBuffer = nullptr;
    return true;
}
void ObjIeeeAscii::WriteHeader()
{
    RenderString("MB" + translatorName + "," + ToString(file->GetName()) + ".");
    endl();
    RenderString("AD" + ObjUtil::ToHex(GetBitsPerMAU()) + "," + ObjUtil::ToHex(GetMAUS()) + "," +
                 (GetFile()->GetBigEndian() ? "M." : "L."));
    endl();
    RenderString("DT" + ToTime(file->GetFileTime()) + ".");
    endl();
    if (file->GetInputFile())
    {
        RenderFile(file->GetInputFile());
    }
    if (absolute)
    {
        RenderComment(eAbsolute,std::move(ObjString("Absolute file")));
    }
}
void ObjIeeeAscii::WriteFiles()
{
    for (auto it = file->SourceFileBegin(); it != file->SourceFileEnd(); ++it)
    {
        RenderFile(*it);
    }
}
void ObjIeeeAscii::WriteSectionHeaders()
{
    for (auto it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        RenderSection(*it);
    }
}
void ObjIeeeAscii::WriteTypes()
{
    if (GetDebugInfoFlag())
    {
        for (auto it = file->TypeBegin(); it != file->TypeEnd(); ++it)
        {
            RenderType(*it);
        }
    }
}
void ObjIeeeAscii::WriteVirtualTypes()
{
    if (GetDebugInfoFlag())
    {
        for (auto it = file->SectionBegin(); it != file->SectionEnd(); ++it)
        {
            ObjType* type = (*it)->GetVirtualType();
            if (type)
            {
                RenderString("AT" + ObjUtil::ToHex((*it)->GetIndex()) + ",T" + GetTypeIndex(type) + ".");
                endl();
            }
        }
    }
}
void ObjIeeeAscii::WriteSymbols()
{
    for (auto it = file->PublicBegin(); it != file->PublicEnd(); ++it)
        RenderSymbol(*it);
    for (auto it = file->ExternalBegin(); it != file->ExternalEnd(); ++it)
        RenderSymbol(*it);
    for (auto it = file->LocalBegin(); it != file->LocalEnd(); ++it)
        RenderSymbol(*it);
    for (auto it = file->AutoBegin(); it != file->AutoEnd(); ++it)
        RenderSymbol(*it);
    for (auto it = file->RegBegin(); it != file->RegEnd(); ++it)
        RenderSymbol(*it);
    for (auto it = file->DefinitionBegin(); it != file->DefinitionEnd(); ++it)
        RenderSymbol(*it);
    for (auto it = file->ImportBegin(); it != file->ImportEnd(); ++it)
        RenderSymbol(*it);
    for (auto it = file->ExportBegin(); it != file->ExportEnd(); ++it)
        RenderSymbol(*it);
}
void ObjIeeeAscii::WriteStartAddress()
{
    if (startAddress)
    {
        RenderCstr("ASG,");
        RenderExpression(startAddress);
        RenderCstr(".");
        endl();
    }
}
void ObjIeeeAscii::WriteSections()
{
    for (auto it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        RenderString("SB" + ObjUtil::ToHex((*it)->GetIndex()) + ".");
        endl();
        RenderMemory(&(*it)->GetMemoryManager());
    }
}
bool ObjIeeeAscii::BinaryWrite()
{
    ioBuffer = std::make_unique<char[]>(BUFFERSIZE);
    for (auto it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        RenderMemoryBinary(&(*it)->GetMemoryManager());
    }
    flush();
    ioBuffer = nullptr;
    return true;
}
void ObjIeeeAscii::WriteBrowseInfo()
{
    for (auto it = file->BrowseInfoBegin(); it != file->BrowseInfoEnd(); ++it)
    {
        RenderBrowseInfo(*it);
    }
}
void ObjIeeeAscii::WriteTrailer()
{
    RenderString("ME.");
    endl();
}
void ObjIeeeAscii::RenderCS()
{
    // the CS is part of the checksum, but the number and '.' are not.
    RenderCstr("CS");
    RenderString(ObjUtil::ToHex(cs & 127, 2) + ".");
    endl();
}
