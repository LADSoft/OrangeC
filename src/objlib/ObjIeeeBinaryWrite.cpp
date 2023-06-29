/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#include "ObjFactory.h"
#include "ObjIeee.h"
#include <cstdio>
#include <sstream>
#include <deque>
#include <cstdarg>
#include <cassert>
#include <climits>

template <typename T>
inline constexpr int min(T x, T y)
{
    return x < y ? x : y;
}
void ObjIeeeBinary::bufferup(const ObjByte* data, int len)
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
void ObjIeeeBinary::RenderMessage(const ObjByte* buf)
{
    GatherCS(buf);
    int l = (buf[1] << 8) + buf[2];
    bufferup(buf, l);
}
void ObjIeeeBinary::RenderMessageInternal(ObjByte* buf, va_list arg)
{
    ObjByte* p = buf + (buf[1] << 8) + buf[2];
    int n;
    while ((n = va_arg(arg, int)))
    {
        if (n & EEMBEDDED)
        {
            *p++ = n & 0xff;
        }
        else
            switch (n)
            {
                case EBYTE:
                    n = va_arg(arg, int);
                    *p++ = n & 0xff;
                    break;
                case EWORD:
                    n = va_arg(arg, int);
                    *p++ = (n >> 8) & 0xff;
                    *p++ = (n >> 0) & 0xff;
                    break;
                case EDWORD:
                    n = va_arg(arg, int);
                    *p++ = (n >> 24) & 0xff;
                    *p++ = (n >> 16) & 0xff;
                    *p++ = (n >> 8) & 0xff;
                    *p++ = (n >> 0) & 0xff;
                    break;
                case EINDEX:
                    n = va_arg(arg, int);
                    if (n <= SHRT_MAX)
                    {
                        *p++ = (n >> 8) | 0x80;
                        *p++ = n & 0xff;
                    }
                    else
                    {
                        *p++ = (n >> 16) & 0xff;
                        *p++ = (n >> 8) & 0xff;
                        *p++ = (n >> 0) & 0xff;
                    }
                    break;
                case ESTRING: {

                    const char* xx = va_arg(arg, const char*);
                    int n = strlen(xx);
                    *p++ = (n >> 8) & 0xff;
                    *p++ = n & 0xff;
                    while (*xx)
                        *p++ = (char)*xx++;
                    break;
                }
                case EBUF: {
                    ObjByte* buf = va_arg(arg, ObjByte*);
                    int n = va_arg(arg, int);
                    memcpy(p, buf, n);
                    p += n;
                }
                break;
                case EEXPR: {
                    ObjExpression* exp = va_arg(arg, ObjExpression*);
                    RenderExpression(buf, exp);
                    ContinueMessage(buf, embed(0x1b), nullptr);
                    p = buf + (buf[1] << 8) + buf[2];
                    break;
                }
                default:
                    assert(0);
                    break;
            }
        int len = p - buf;
        buf[1] = (len >> 8) & 0xff;
        buf[2] = len & 0xff;
    }
}
ObjByte* ObjIeeeBinary::StartMessage(int msg, ...)
{
    static ObjByte buf[20000];
    ObjByte* p = buf;
    *p++ = msg;
    *p++ = 0;
    *p++ = 3;
    va_list arg;
    va_start(arg, msg);
    RenderMessageInternal(buf, arg);
    va_end(arg);
    return buf;
}
void ObjIeeeBinary::ContinueMessage(ObjByte* buf, ...)
{
    va_list arg;
    va_start(arg, buf);
    RenderMessageInternal(buf, arg);
    va_end(arg);
}
void ObjIeeeBinary::RenderMessage(int msg, ...)
{
    static ObjByte buf[4096];
    ObjByte* p = buf;
    *p++ = msg;
    *p++ = 0;
    *p++ = 3;
    va_list arg;
    va_start(arg, msg);
    RenderMessageInternal(buf, arg);
    va_end(arg);
    RenderMessage(buf);
}
void ObjIeeeBinary::RenderComment(int tp, ...)
{
    static ObjByte buf[4096];
    ObjByte* p = buf;
    *p++ = ecCO;
    *p++ = 0;
    *p++ = 5;
    *p++ = (tp >> 8) & 0xff;
    *p++ = tp & 0xff;
    va_list arg;
    va_start(arg, tp);
    RenderMessageInternal(buf, arg);
    va_end(arg);
    RenderMessage(buf);
}
int ObjIeeeBinary::GetSymbolName(ObjSymbol* Symbol)
{
    int name;
    switch (Symbol->GetType())
    {
        case ObjSymbol::ePublic:
        default:
            name = 'I';
            break;
        case ObjSymbol::eExternal:
            name = 'X';
            break;
        case ObjSymbol::eLocal:
            name = 'N';
            break;
        case ObjSymbol::eAuto:
            name = 'A';
            break;
        case ObjSymbol::eReg:
            name = 'E';
            break;
    }
    return name;
}
ObjString ObjIeeeBinary::ToTime(std::tm tms)
{
    ObjString rv;
    rv = ObjUtil::ToDecimal(tms.tm_year + 1900, 4) + ObjUtil::ToDecimal(tms.tm_mon + 1, 2) + ObjUtil::ToDecimal(tms.tm_mday, 2) +
         ObjUtil::ToDecimal(tms.tm_hour, 2) + ObjUtil::ToDecimal(tms.tm_min, 2) + ObjUtil::ToDecimal(tms.tm_sec, 2);
    return rv;
}
void ObjIeeeBinary::RenderFile(ObjSourceFile* File)
{
    RenderComment(eSourceFile, EINDEX, File->GetIndex(), ESTRING, File->GetName().c_str(), ESTRING,
                  ToTime(File->GetFileTime()).c_str(), nullptr);
}
int ObjIeeeBinary::GetTypeIndex(ObjType* Type)
{
    if (Type->GetType() < ObjType::eVoid)
        return Type->GetIndex();
    else
        return (int)Type->GetType();
}
void ObjIeeeBinary::RenderStructure(ObjType* Type)
{
    const int MaxPerLine = 15;
    std::deque<ObjField*> fields;
    for (ObjType::FieldIterator it = Type->FieldBegin(); it != Type->FieldEnd(); ++it)
    {
        fields.push_front(*it);
    }
    int lastIndex = -1;
    while (fields.size())
    {
        ObjByte* buf;
        int bottom;
        int index;
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
            buf = StartMessage(ecAT, embed('T'), EINDEX, index = Type->GetIndex(), EINDEX, Type->GetType(), EDWORD, Type->GetSize(),
                               nullptr);
        }
        else
        {
            buf = StartMessage(ecAT, embed('T'), EINDEX, index = current->GetTypeIndex(), EINDEX, ObjType::eField, nullptr);
        }
        for (unsigned j = 0; j < bottom; j++)
        {
            ObjField* currentField = fields[bottom - j - 1];
            ContinueMessage(buf, EINDEX, GetTypeIndex(currentField->GetBase()), ESTRING, currentField->GetName().c_str(), EDWORD,
                            currentField->GetConstVal(), nullptr);
        }
        if (lastIndex != -1)
        {
            ContinueMessage(buf, EINDEX, lastIndex, nullptr);
        }
        RenderMessage(buf);
        lastIndex = index;
        for (unsigned j = 0; j < bottom; j++)
            fields.pop_front();
    }
}
void ObjIeeeBinary::RenderFunction(ObjFunction* Function)
{
    ObjByte* buf = StartMessage(ecAT, embed('T'), EINDEX, GetTypeIndex(static_cast<ObjType*>(Function)), EINDEX, ObjType::eFunction,
                                EINDEX, GetTypeIndex(Function->GetReturnType()), EDWORD, Function->GetLinkage(), nullptr);
    // assuming a reasonable number of parameters
    // parameters are TYPES
    for (ObjFunction::ParameterIterator it = Function->ParameterBegin(); it != Function->ParameterEnd(); ++it)
    {
        ContinueMessage(buf, EINDEX, GetTypeIndex(*it), nullptr);
    }
    RenderMessage(buf);
}
void ObjIeeeBinary::RenderType(ObjType* Type)
{
    switch (Type->GetType())
    {
        case ObjType::ePointer:
        case ObjType::eLRef:
        case ObjType::eRRef:
            RenderMessage(ecAT, embed('T'), EINDEX, GetTypeIndex(Type), EINDEX, ObjType::ePointer, EDWORD, Type->GetSize(), EINDEX,
                          GetTypeIndex(Type->GetBaseType()), nullptr);
            break;
        case ObjType::eTypeDef:
            RenderMessage(ecAT, embed('T'), EINDEX, GetTypeIndex(Type), EINDEX, ObjType::eTypeDef, EINDEX,
                          GetTypeIndex(Type->GetBaseType()), nullptr);
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
            RenderMessage(ecAT, embed('T'), EINDEX, GetTypeIndex(Type), EINDEX, ObjType::eBitField, EBYTE, Type->GetSize(), EINDEX,
                          GetTypeIndex(Type->GetBaseType()), EBYTE, Type->GetStartBit(), EBYTE, Type->GetBitCount(), nullptr);
            break;
        case ObjType::eArray:
            RenderMessage(ecAT, embed('T'), EINDEX, GetTypeIndex(Type), EINDEX, Type->GetType(), EDWORD, Type->GetSize(), EINDEX,
                          GetTypeIndex(Type->GetBaseType()), EINDEX, GetTypeIndex(Type->GetIndexType()), EDWORD, Type->GetBase(),
                          EDWORD, Type->GetTop(), nullptr);
            break;
        case ObjType::eVla:
            RenderMessage(ecAT, embed('T'), EINDEX, GetTypeIndex(Type), EINDEX, Type->GetType(), EDWORD, Type->GetSize(), EINDEX,
                          GetTypeIndex(Type->GetBaseType()), EINDEX, GetTypeIndex(Type->GetIndexType()), nullptr);
            break;
    }
    if (Type->GetType() < ObjType::eVoid && Type->GetName().size())
    {
        RenderMessage(ecNAME, embed('T'), EINDEX, Type->GetIndex(), ESTRING, Type->GetName().c_str(), nullptr);
    }
}
void ObjIeeeBinary::RenderSymbol(ObjSymbol* Symbol)
{
    if (Symbol->GetType() == ObjSymbol::eDefinition)
    {
        ObjDefinitionSymbol* dsym = static_cast<ObjDefinitionSymbol*>(Symbol);
        RenderComment(eDefinition, ESTRING, dsym->GetName().c_str(), EDWORD, dsym->GetValue(), nullptr);
    }
    else if (Symbol->GetType() == ObjSymbol::eImport)
    {
        ObjImportSymbol* isym = static_cast<ObjImportSymbol*>(Symbol);
        if (isym->GetByOrdinal())
            RenderComment(eImport, embed('O'), ESTRING, isym->GetName().c_str(), EDWORD, isym->GetOrdinal(), ESTRING,
                          isym->GetDllName().c_str(), nullptr);
        else
            RenderComment(eImport, embed('N'), ESTRING, isym->GetName().c_str(), ESTRING, isym->GetExternalName().c_str(), ESTRING,
                          isym->GetDllName().c_str(), nullptr);
    }
    else if (Symbol->GetType() == ObjSymbol::eExport)
    {
        ObjString data;
        ObjExportSymbol* esym = static_cast<ObjExportSymbol*>(Symbol);
        if (esym->GetByOrdinal())
            RenderComment(eExport, embed('O'), ESTRING, esym->GetName().c_str(), EDWORD, esym->GetOrdinal(), ESTRING,
                          esym->GetDllName().c_str(), nullptr);
        else
            RenderComment(eExport, embed('N'), ESTRING, esym->GetName().c_str(), ESTRING, esym->GetExternalName().c_str(), ESTRING,
                          esym->GetDllName().c_str(), nullptr);
    }
    else if (Symbol->GetType() != ObjSymbol::eLabel)
    {
        RenderMessage(ecNAME, embed(GetSymbolName(Symbol)), EINDEX, Symbol->GetIndex(), ESTRING, Symbol->GetName().c_str(),
                      nullptr);
        if (Symbol->GetOffset())
        {
            RenderMessage(ecAS, embed(GetSymbolName(Symbol)), EINDEX, Symbol->GetIndex(), EEXPR, Symbol->GetOffset(), nullptr);
        }
        if (GetDebugInfoFlag() && Symbol->GetBaseType())
        {
            RenderMessage(ecAT, embed(GetSymbolName(Symbol)), EINDEX, Symbol->GetIndex(), EINDEX,
                          GetTypeIndex(Symbol->GetBaseType()), nullptr);
        }
    }
}
void ObjIeeeBinary::RenderSection(ObjSection* Section)
{
    // This is actually the section header information
    // this assums a section number < 160... otherwise it could be an attrib
    RenderMessage(ecST, EINDEX, Section->GetIndex(), EDWORD, Section->GetQuals(), ESTRING, Section->GetName().c_str(), nullptr);

    RenderMessage(ecSA, EINDEX, Section->GetIndex(), EDWORD, Section->GetAlignment(), nullptr);
    RenderMessage(ecAS, embed('S'), EINDEX, Section->GetIndex(), embed('V'), EDWORD, Section->GetMemoryManager().GetSize(),
                  embed(0x1b), nullptr);
    if (Section->GetVirtualType())
    {
        int n = Section->GetVirtualType()->GetIndex();
        if (n < ObjType::eReservedTop + 1)
            n = Section->GetVirtualType()->GetType();
        RenderMessage(ecAT, embed('R'), EINDEX, Section->GetIndex(), EINDEX, n, nullptr);
    }
    if (Section->GetQuals() & ObjSection::absolute)
    {
        RenderMessage(ecAS, embed('L'), EINDEX, Section->GetIndex(), embed('V'), EDWORD, Section->GetMemoryManager().GetBase(),
                      embed(0x1b), nullptr);
    }
}
void ObjIeeeBinary::RenderDebugTag(ObjDebugTag* Tag)
{
    ObjString data;
    switch (Tag->GetType())
    {
        case ObjDebugTag::eVar:
            /* debugger has to dereference the name */
            if (!Tag->GetSymbol()->IsSectionRelative() && Tag->GetSymbol()->GetType() != ObjSymbol::eExternal)
            {
                RenderComment(eVar, embed(GetSymbolName(Tag->GetSymbol())), EINDEX, Tag->GetSymbol()->GetIndex(), nullptr);
            }
            break;
        case ObjDebugTag::eBlockStart:
        case ObjDebugTag::eBlockEnd:
            RenderComment(Tag->GetType() == ObjDebugTag::eBlockStart ? eBlockStart : eBlockEnd, ESTRING, "", nullptr);
            break;
        case ObjDebugTag::eFunctionStart:
        case ObjDebugTag::eFunctionEnd:
            RenderComment(Tag->GetType() == ObjDebugTag::eFunctionStart ? eFunctionStart : eFunctionEnd,
                          embed(GetSymbolName(Tag->GetSymbol())), EINDEX, Tag->GetSymbol()->GetIndex(), nullptr);
            break;
        case ObjDebugTag::eVirtualFunctionStart:
        case ObjDebugTag::eVirtualFunctionEnd:
            RenderComment(Tag->GetType() == ObjDebugTag::eVirtualFunctionStart ? eFunctionStart : eFunctionEnd, embed('R'), EINDEX,
                          Tag->GetSection()->GetIndex(), nullptr);
            break;
        case ObjDebugTag::eLineNo:
            RenderComment(eLineNo, EINDEX, Tag->GetLineNo()->GetFile()->GetIndex(), EDWORD, Tag->GetLineNo()->GetLineNumber(),
                          nullptr);
            break;
        default:
            break;
    }
}
void ObjIeeeBinary::RenderMemory(ObjMemoryManager* Memory)
{
    // this function is optimized to not use C++ stream objects
    // because it is called a lot, and the resultant memory allocations
    // really slow down linker and librarian operations
    char scratch[256];
    int n = 0;
    ObjMemoryManager::MemoryIterator itmem;
    for (itmem = Memory->MemoryBegin(); itmem != Memory->MemoryEnd(); ++itmem)
    {
        ObjMemory* memory = (*itmem);
        if (memory->HasDebugTags() && GetDebugInfoFlag() || memory->GetFixup())
        {
            if (n)
                RenderMessage(ecLD, EBUF, scratch, n, nullptr);
            n = 0;
            if (GetDebugInfoFlag() && memory->HasDebugTags())
            {
                ObjMemory::DebugTagIterator it;
                for (it = memory->DebugTagBegin(); it != memory->DebugTagEnd(); ++it)
                {
                    RenderDebugTag(*it);
                }
            }
            if (memory->GetFixup())
            {
                RenderMessage(ecLR, EEXPR, memory->GetFixup(), EBYTE, memory->GetSize(), nullptr);
            }
        }
        if (memory->IsEnumerated())
        {
            if (n)
                RenderMessage(ecLD, EBUF, scratch, n, nullptr);
            n = 0;
            RenderMessage(ecLE, EDWORD, memory->GetSize(), EBYTE, memory->GetFill(), nullptr);
        }
        else if (memory->GetData())
        {
            ObjByte* p = memory->GetData();
            for (int i = 0; i < memory->GetSize(); i++)
            {
                scratch[n++] = *p++;
                if (n >= 256)
                {
                    RenderMessage(ecLD, EBUF, scratch, n, nullptr);
                    n = 0;
                }
            }
        }
    }
    if (n)
        RenderMessage(ecLD, EBUF, scratch, n, nullptr);
    n = 0;
}
void ObjIeeeBinary::RenderMemoryBinary(ObjMemoryManager* Memory)
{
    ObjByte scratch[256];
    for (auto itmem = Memory->MemoryBegin(); itmem != Memory->MemoryEnd(); ++itmem)
    {
        ObjMemory* memory = (*itmem);
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
            bufferup((ObjByte*)memory->GetData(), memory->GetSize());
        }
    }
}
void ObjIeeeBinary::RenderBrowseInfo(ObjBrowseInfo* BrowseInfo)
{
    RenderComment(eBrowseInfo, EINDEX, BrowseInfo->GetType(), EDWORD, BrowseInfo->GetQual(), EINDEX,
                  BrowseInfo->GetLineNo()->GetFile()->GetIndex(), EDWORD, BrowseInfo->GetLineNo()->GetLineNumber(), EWORD,
                  BrowseInfo->GetCharPos(), ESTRING, BrowseInfo->GetData().c_str(), nullptr);
}
void ObjIeeeBinary::RenderExpression(ObjByte* buf, ObjExpression* Expression)
{
    switch (Expression->GetOp())
    {
        case ObjExpression::eNop:
            break;
        case ObjExpression::eNonExpression:
            RenderExpression(buf, Expression->GetLeft());
            RenderExpression(buf, Expression->GetRight());
            break;
        case ObjExpression::eValue:
            ContinueMessage(buf, embed('V'), EDWORD, Expression->GetValue(), nullptr);
            break;
        case ObjExpression::eAdd:
            RenderExpression(buf, Expression->GetLeft());
            if (Expression->GetRight()->GetOp() != ObjExpression::eValue || Expression->GetRight()->GetValue() != 0)
            {
                RenderExpression(buf, Expression->GetRight());
                ContinueMessage(buf, embed('+'), nullptr);
            }
            break;
        case ObjExpression::eSub:
            RenderExpression(buf, Expression->GetLeft());
            RenderExpression(buf, Expression->GetRight());
            ContinueMessage(buf, embed('-'), nullptr);
            break;
        case ObjExpression::eMul:
            RenderExpression(buf, Expression->GetLeft());
            RenderExpression(buf, Expression->GetRight());
            ContinueMessage(buf, embed('*'), nullptr);
            break;
        case ObjExpression::eDiv:
            RenderExpression(buf, Expression->GetLeft());
            RenderExpression(buf, Expression->GetRight());
            ContinueMessage(buf, embed('/'), nullptr);
            break;
        case ObjExpression::eExpression:
            RenderExpression(buf, Expression->GetLeft());
            break;
        case ObjExpression::eSymbol:
            if (Expression->GetSymbol()->GetType() == ObjSymbol::eExternal)
            {
                // externals get embedded in the expression
                ContinueMessage(buf, embed('X'), EINDEX, Expression->GetSymbol()->GetIndex(), nullptr);
            }
            else
            {
                // other types of symbols we don't embed in the expression,
                // instead we embed their values
                RenderExpression(buf, Expression->GetSymbol()->GetOffset());
            }
            break;
        case ObjExpression::eSection:
            ContinueMessage(buf, embed('R'), EINDEX, Expression->GetSection()->GetIndex(), nullptr);
            break;
        case ObjExpression::ePC:
            ContinueMessage(buf, embed('P'), nullptr);
            break;
    }
}
void ObjIeeeBinary::GatherCS(const ObjByte* msg)
{
    int len = (msg[1] << 8) + msg[2];
    for (int i = 0; i < len; i++)
        cs += msg[i];
}
bool ObjIeeeBinary::HandleWrite()
{
    ioBufferLen = 0;
    ioBuffer = std::make_unique<char[]>(BUFFERSIZE);
    ResetCS();
    WriteHeader();
    RenderCS();
    ResetCS();
    WriteFiles();
    RenderComment(eMakePass, ESTRING, "Make Pass Separator", nullptr);
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
    RenderComment(eLinkPass, ESTRING, "Link Pass Separator", nullptr);
    WriteSections();
    RenderCS();
    ResetCS();
    RenderComment(eBrowsePass, ESTRING, "Browse Pass Separator", nullptr);
    WriteBrowseInfo();
    RenderCS();
    ResetCS();
    WriteTrailer();
    flush();
    ioBuffer = nullptr;
    return true;
}
void ObjIeeeBinary::WriteHeader()
{
    RenderMessage(ecMB, ESTRING, translatorName.c_str(), ESTRING, file->GetName().c_str(), nullptr);
    RenderMessage(ecAD, EBYTE, GetBitsPerMAU(), EBYTE, GetMAUS(), EBYTE, embed(GetFile()->GetBigEndian() ? 'M' : 'L'), nullptr);
    RenderMessage(ecDT, ESTRING, ToTime(file->GetFileTime()).c_str(), nullptr);
    if (file->GetInputFile())
    {
        RenderFile(file->GetInputFile());
    }
    if (absolute)
    {
        RenderComment(eAbsolute, ESTRING, "Link Pass Separator", nullptr);
    }
}
void ObjIeeeBinary::WriteFiles()
{
    for (ObjFile::SourceFileIterator it = file->SourceFileBegin(); it != file->SourceFileEnd(); ++it)
    {
        RenderFile(*it);
    }
}
void ObjIeeeBinary::WriteSectionHeaders()
{
    for (ObjFile::SectionIterator it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        RenderSection(*it);
    }
}
void ObjIeeeBinary::WriteTypes()
{
    if (GetDebugInfoFlag())
    {
        for (ObjFile::TypeIterator it = file->TypeBegin(); it != file->TypeEnd(); ++it)
        {
            RenderType(*it);
        }
    }
}
void ObjIeeeBinary::WriteSymbols()
{
    for (ObjFile::SymbolIterator it = file->PublicBegin(); it != file->PublicEnd(); ++it)
        RenderSymbol(*it);
    for (ObjFile::SymbolIterator it = file->ExternalBegin(); it != file->ExternalEnd(); ++it)
        RenderSymbol(*it);
    for (ObjFile::SymbolIterator it = file->LocalBegin(); it != file->LocalEnd(); ++it)
        RenderSymbol(*it);
    for (ObjFile::SymbolIterator it = file->AutoBegin(); it != file->AutoEnd(); ++it)
        RenderSymbol(*it);
    for (ObjFile::SymbolIterator it = file->RegBegin(); it != file->RegEnd(); ++it)
        RenderSymbol(*it);
    for (ObjFile::SymbolIterator it = file->DefinitionBegin(); it != file->DefinitionEnd(); ++it)
        RenderSymbol(*it);
    for (ObjFile::SymbolIterator it = file->ImportBegin(); it != file->ImportEnd(); ++it)
        RenderSymbol(*it);
    for (ObjFile::SymbolIterator it = file->ExportBegin(); it != file->ExportEnd(); ++it)
        RenderSymbol(*it);
}
void ObjIeeeBinary::WriteStartAddress()
{
    if (startAddress)
    {
        RenderMessage(ecAS, embed('G'), EEXPR, startAddress, nullptr);
    }
}
void ObjIeeeBinary::WriteSections()
{
    for (ObjFile::SectionIterator it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        RenderMessage(ecSB, EINDEX, (*it)->GetIndex(), nullptr);
        RenderMemory(&(*it)->GetMemoryManager());
    }
}
bool ObjIeeeBinary::BinaryWrite()
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
void ObjIeeeBinary::WriteBrowseInfo()
{
    for (ObjFile::BrowseInfoIterator it = file->BrowseInfoBegin(); it != file->BrowseInfoEnd(); ++it)
    {
        RenderBrowseInfo(*it);
    }
}
void ObjIeeeBinary::WriteTrailer() { RenderMessage(ecME, nullptr); }
void ObjIeeeBinary::RenderCS()
{
    // the CS is part of the checksum, but the number and '.' are not.
    RenderMessage(ecCS, EBYTE, cs + ecCS, nullptr);
}
void ObjIeeeIndexManager::ResetIndexes()
{
    Section = 0;
    Public = 0;
    Local = 0;
    External = 0;
    Type = eDerivedTypeBase;
    File = 0;
    Auto = 0;
    Reg = 0;
}
