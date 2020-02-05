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

#include "ObjFactory.h"
#include "ObjIeee.h"
#include <stdio.h>
#include <sstream>
#include <deque>
#include <stdarg>
#include <assert.h>

inline int min(int x,int y)
{
    if (x < y) 
        return x;
    else
        return y;
}
void ObjIeeeBinary::bufferup(const char *data, int len)
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
        memcpy(ioBuffer + ioBufferLen, data, len);
        ioBufferLen += len;
    }
}
void ObjIeeeBinary::RenderMessage(const ObjByte *buf)
{
    GatherCS(buf);
    int l = buf[1] + (buf[2] << 8);
    bufferup(buf, l);
}
void vv()
{
}
void ObjIeeeBinary::RenderMessageInternal(ObjByte *buf, va_list arg)
{
    ObjByte *p = buf + (buf[1]) + (buf[2] << 8);
    int n;
    while (n = va_arg(arg, int))
    {
        if (n & EEMBEDDED)
        {
            *p++ = n & 0xff;
        }
        else switch (n)
        {
            case EBYTE:
                n = va_arg(arg, int);
                *p++ = n & 0xff;
                break;
            case EWORD:
                n = va_arg(arg, int);
                *p++ = n & 0xff;
                *p++ = (n >> 8) & 0xff;
                break;
            case EDWORD:
                n = va_arg(arg, int);
                *p++ = n & 0xff;
                *p++ = (n >> 8) & 0xff;
                *p++ = (n >> 16) & 0xff;
                *p++ = (n >> 24) & 0xff;
                break;
            case ESTRING:
            {
                std::string xx = va_arg(arg, ObjString);
                *p++ = xx.length();
                for (int i=0; i < xx.length(); i++)
                    *p++ = xx[i];
                break;
            }
            case EBUF:
            {
                ObjByte *buf = va_arg(arg, ObjByte *);
                int n = va_arg(arg, int);
                memcpy(p, buf, n);
                p += n;
            }
                break;
            case EEXPR:
            {
                vv();
                ObjExpression *exp = va_arg(arg, ObjExpression *);   
                RenderExpression(buf, exp);
                ContinueMessage(buf, embed(0x1b), NULL);
                p = buf + (buf[1]) + (buf[2] << 8);
                break;
            }
            default:
                assert(0);
                break;
        }
        int len = p - buf;
        buf[1] = len & 0xff;
        buf[2] = (len >> 8) & 0xff;
    }
    
}
ObjByte *ObjIeeeBinary::StartMessage(eCommands msg, ...)
{
    static ObjByte buf[4096];
    ObjByte *p = buf;
    *p++ = msg;
    *p++ = 3;
    *p++ = 0;
    va_list arg;
    va_start(arg, msg);
    RenderMessageInternal(buf, arg);
    va_end(arg);
    return buf;
}
void ObjIeeeBinary::ContinueMessage(ObjByte *buf, ...)
{
    va_list arg;
    va_start(arg, buf);
    RenderMessageInternal(buf, arg);
    va_end(arg);
}
void ObjIeeeBinary::RenderMessage(eCommands msg, ...)
{
    static ObjByte buf[4096];
    ObjByte *p = buf;
    *p++ = msg;
    *p++ = 3;
    *p++ = 0;
    va_list arg;
    va_start(arg, msg);
    RenderMessageInternal(buf, arg);
    va_end(arg);
    RenderMessage(buf);
}
void ObjIeeeBinary::RenderComment(eCommentType tp, ...)
{
    static ObjByte buf[4096];
    ObjByte *p = buf;
    *p++ = ecCO;
    *p++ = 5;
    *p++ = 0;
    *p++ = tp & 0xff;
    *p++ = (tp >> 8) & 0xff;
    va_list arg;
    va_start(arg, tp);
    RenderMessageInternal(buf, arg);
    va_end(arg);
    RenderMessage(buf);
}
int ObjIeeeBinary::GetSymbolName(ObjSymbol *Symbol)
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
    ObjString rv ;
    rv = ObjUtil::ToDecimal(tms.tm_year + 1900,4) +
                ObjUtil::ToDecimal(tms.tm_mon + 1,2) +
                ObjUtil::ToDecimal(tms.tm_mday,2) +
                ObjUtil::ToDecimal(tms.tm_hour,2) +
                ObjUtil::ToDecimal(tms.tm_min,2) +
                ObjUtil::ToDecimal(tms.tm_sec,2) ;
    return rv;
}
void ObjIeeeBinary::RenderFile(ObjSourceFile *File)
{
    RenderComment(eSourceFile, EDWORD, File->GetIndex(), ESTRING, File->GetName(), ESTRING, ToTime(File->GetFileTime()), NULL);
}
int ObjIeeeBinary::GetTypeIndex(ObjType *Type)
{
    if (Type->GetType() < ObjType::eVoid)
        return Type->GetIndex();
    else
        return (int)Type->GetType();
}
void ObjIeeeBinary::RenderStructure(ObjType *Type)
{
    const int MaxPerLine = 15;
    std::deque<ObjField *> fields;
    for (ObjType::FieldIterator it = Type->FieldBegin(); it != Type->FieldEnd(); ++it)
    {
        fields.push_front(*it);
    }
    int lastIndex = -1;
    while (fields.size())
    {
        unsigned n = min(MaxPerLine, (int)fields.size());
        int index;
        // the problem with this is if they output the file twice
        // the types won't match
        ObjByte *buf;
        if (n == fields.size())
        {
            buf = StartMessage(ecAT, embed('T'), EDWORD, index = Type->GetIndex(), EDWORD, Type->GetType(), EDWORD, Type->GetSize(), NULL);
        }
        else
        {
            buf = StartMessage(ecAT, embed('T'), EDWORD, index = GetFactory()->GetIndexManager()->NextType(), EDWORD, ObjType::eField, NULL);
        }
        for (unsigned j=0; j < n; j++)
        {
            ObjField *currentField = fields[n-1-j];
            ContinueMessage(buf,  EDWORD, GetTypeIndex(currentField->GetBase()),
                            ESTRING, currentField->GetName(), 
                            EDWORD, currentField->GetConstVal(), NULL);
        }
        if (lastIndex != -1)
            ContinueMessage(buf, EDWORD, lastIndex, NULL); 
        lastIndex = index;
        RenderMessage(buf);
        for (unsigned j=0; j < n; j++)
            fields.pop_front();
    }
}
void ObjIeeeBinary::RenderFunction(ObjFunction *Function)
{
    ObjByte*buf = StartMessage(ecAT, embed('T'), EDWORD, GetTypeIndex(static_cast<ObjType *>(Function)),
                             EDWORD, ObjType::eFunction, EDWORD, GetTypeIndex(Function->GetReturnType()),
                             EDWORD, Function->GetLinkage(), NULL);
    // assuming a reasonable number of parameters
    // parameters are TYPES
    for (ObjFunction::ParameterIterator it = Function->ParameterBegin();
         it != Function->ParameterEnd(); ++it)
    {
        ContinueMessage(buf, EDWORD, GetTypeIndex(*it), NULL);
    }
    RenderMessage(buf);
}
void ObjIeeeBinary::RenderType(ObjType *Type)
{
    if (Type->GetType() < ObjType::eVoid && Type->GetName().size())
    {
        RenderMessage(ecNAME, embed('T'), EDWORD, Type->GetIndex(),
                      ESTRING, Type->GetName(), NULL);
    }
    switch(Type->GetType())
    {
        case ObjType::ePointer:
            RenderMessage(ecAT, embed('T'), EDWORD, GetTypeIndex(Type),
                          EDWORD, ObjType::ePointer,
                          EDWORD, Type->GetSize(),
                          EDWORD, GetTypeIndex(Type->GetBaseType()), NULL);
            break;
        case ObjType::eTypeDef:
            RenderMessage(ecAT, embed('T'), EDWORD, GetTypeIndex(Type),
                          EDWORD, ObjType::eTypeDef,
                          EDWORD, GetTypeIndex(Type->GetBaseType()), NULL);
            break;
        case ObjType::eFunction:
            RenderFunction(static_cast<ObjFunction *>(Type));
            break;
        case ObjType::eStruct:
        case ObjType::eUnion:
        case ObjType::eEnum:
            RenderStructure(Type);
            break;
        case ObjType::eBitField:
            RenderMessage(ecAT, embed('T'), EDWORD, GetTypeIndex(Type),
                          EDWORD, ObjType::eBitField,
                          EBYTE, Type->GetSize(),
                          EDWORD, GetTypeIndex(Type->GetBaseType()),
                          EBYTE, Type->GetStartBit(),
                          EBYTE, Type->GetBitCount(), NULL);
            break;
        case ObjType::eArray:
            RenderMessage(ecAT, embed('T'), EDWORD, GetTypeIndex(Type),
                          EDWORD, Type->GetType(),
                          EDWORD, Type->GetSize(),
                          EDWORD, GetTypeIndex(Type->GetBaseType()),
                          EDWORD, GetTypeIndex(Type->GetIndexType()),
                          EDWORD, Type->GetBase(),
                          EDWORD, Type->GetTop(), NULL);
            break;
        case ObjType::eVla:
            RenderMessage(ecAT, embed('T'), EDWORD, GetTypeIndex(Type),
                          EDWORD, Type->GetType(),
                          EDWORD, Type->GetSize(),
                          EDWORD, GetTypeIndex(Type->GetBaseType()),
                          EDWORD, GetTypeIndex(Type->GetIndexType()), NULL);
            break;
    }
}
void ObjIeeeBinary::RenderSymbol(ObjSymbol *Symbol)
{
    if (Symbol->GetType() == ObjSymbol::eDefinition)
    {
        ObjDefinitionSymbol *dsym = static_cast<ObjDefinitionSymbol *>(Symbol);
        RenderComment(eDefinition, ESTRING, dsym->GetName(), EDWORD, dsym->GetValue(), NULL);
    }
    else if (Symbol->GetType() == ObjSymbol::eImport)
    {
        ObjImportSymbol *isym = static_cast<ObjImportSymbol *>(Symbol);
        if (isym->GetByOrdinal())
            RenderComment(eImport, embed('O'), ESTRING, isym->GetName(), 
                          EDWORD, isym->GetOrdinal(), ESTRING, isym->GetDllName(), NULL);
        else
            RenderComment(eImport, embed('N'), ESTRING, isym->GetName(), 
                          ESTRING, isym->GetExternalName(), ESTRING, isym->GetDllName(), NULL);
    }
    else if (Symbol->GetType() == ObjSymbol::eExport)
    {
        ObjString data;
        ObjExportSymbol *esym = static_cast<ObjExportSymbol *>(Symbol);
        if (esym->GetByOrdinal())
            RenderComment(eExport, embed('O'), ESTRING, esym->GetName(), 
                          EDWORD, esym->GetOrdinal(), ESTRING, esym->GetDllName(), NULL);
        else
            RenderComment(eExport, embed('N'), ESTRING, esym->GetName(), 
                          ESTRING, esym->GetExternalName(), ESTRING, esym->GetDllName(), NULL);
    }
    else if (Symbol->GetType() != ObjSymbol::eLabel)
    {
        RenderMessage(ecNAME, embed(GetSymbolName(Symbol)), EDWORD, Symbol->GetIndex(), 
                      ESTRING, Symbol->GetName(), NULL);
        if (Symbol->GetOffset())
        {
           RenderMessage(ecAS, embed(GetSymbolName(Symbol)), EDWORD, Symbol->GetIndex(), 
                         EEXPR, Symbol->GetOffset(), NULL);
        }
        if (GetDebugInfoFlag() && Symbol->GetBaseType())
        {
            RenderMessage(ecAT, embed(GetSymbolName(Symbol)), EDWORD, Symbol->GetIndex(), 
                         EDWORD, GetTypeIndex(Symbol->GetBaseType()), NULL);
        }
    }
}
void ObjIeeeBinary::RenderSection(ObjSection *Section)
{
    // This is actually the section header information
    // this assums a section number < 160... otherwise it could be an attrib
    RenderMessage(ecST, EDWORD, Section->GetIndex(), 
                  EDWORD, Section->GetQuals(), 
                  ESTRING, Section->GetName(), NULL);

    RenderMessage(ecSA, EDWORD, Section->GetIndex(),
                        EDWORD, Section->GetAlignment(), NULL);
    RenderMessage(ecAS, embed('S'), EDWORD, Section->GetIndex(),
                              embed('V'), EDWORD, Section->GetMemoryManager().GetSize(), embed(0x1b), NULL);
    if (Section->GetQuals() & ObjSection::absolute)
    {
        RenderMessage(ecAS, embed('L'), EDWORD, Section->GetIndex(),
                              embed('V'), EDWORD, Section->GetMemoryManager().GetBase(), embed(0x1b), NULL);
    }
}
void ObjIeeeBinary::RenderDebugTag(ObjDebugTag *Tag)
{
    ObjString data;
    switch(Tag->GetType())
    {
        case ObjDebugTag::eVar:
            /* debugger has to dereference the name */
            if (!Tag->GetSymbol()->IsSectionRelative() && 
                Tag->GetSymbol()->GetType() != ObjSymbol::eExternal)
            {
                RenderComment(eVar, embed(GetSymbolName(Tag->GetSymbol())),
                              EDWORD, Tag->GetSymbol()->GetIndex(), NULL);
            }
            break;
        case ObjDebugTag::eBlockStart:
        case ObjDebugTag::eBlockEnd:
            RenderComment(Tag->GetType() == ObjDebugTag::eBlockStart ?
                                 eBlockStart : eBlockEnd,
                                 ESTRING, ObjString(""), NULL);
            break;
        case ObjDebugTag::eFunctionStart:
        case ObjDebugTag::eFunctionEnd:
            RenderComment(eFunctionStart, embed(GetSymbolName(Tag->GetSymbol())),
                          EDWORD, Tag->GetSymbol()->GetIndex(), NULL);
            break;
        case ObjDebugTag::eLineNo:
            RenderComment(eLineNo, 
                          EDWORD, Tag->GetLineNo()->GetFile()->GetIndex(),
                          EDWORD, Tag->GetLineNo()->GetLineNumber(), NULL);
            break;
        default:
            break;
    }
}
void ObjIeeeBinary::RenderMemory(ObjMemoryManager *Memory)
{
    // this function is optimized to not use C++ stream objects
    // because it is called a lot, and the resultant memory allocations
    // really slow down linker and librarian operations
    char scratch[256];
    int n = 0;
    ObjMemoryManager::MemoryIterator itmem;
    for (itmem = Memory->MemoryBegin(); 
         itmem != Memory->MemoryEnd(); ++itmem)
    {
        ObjMemory *memory = (*itmem);
        if (memory->HasDebugTags() && GetDebugInfoFlag()
            || memory->GetFixup())
        {
            if (n)
                RenderMessage(ecLD, EBUF, scratch, n, NULL);
            n = 0;
            if (GetDebugInfoFlag())
            {
                ObjMemory::DebugTagIterator it;
                for (it = memory->DebugTagBegin(); it != memory->DebugTagEnd(); ++it)
                {
                    RenderDebugTag(*it);
                }
            }
            if (memory->GetFixup())
            {
                RenderMessage(ecLR, EEXPR, memory->GetFixup(), EBYTE, memory->GetSize(), NULL);
            }
        }
        if (memory->IsEnumerated())
        {
            if (n)
                RenderMessage(ecLD, EBUF, scratch, n, NULL);
            n = 0;
            RenderMessage(ecLE, EDWORD, memory->GetSize(),
                          EBYTE, memory->GetFill(), NULL);
        }
        else if (memory->GetData())
        {
            ObjByte *p = memory->GetData();
            for (int i=0; i < memory->GetSize(); i++)
            {
                scratch[n++] = *p++;
                if (n >= 256)
                {
                    RenderMessage(ecLD, EBUF, scratch, n, NULL);
                    n = 0;
                }
            }
        }
    }
    if (n)
        RenderMessage(ecLD, EBUF, scratch, n, NULL);
    n = 0;
}
void ObjIeeeBinary::RenderBrowseInfo(ObjBrowseInfo *BrowseInfo)
{
    RenderComment(eBrowseInfo,
                  EDWORD, BrowseInfo->GetType(),
                  EDWORD, BrowseInfo->GetQual(),
                  EDWORD, BrowseInfo->GetLineNo()->GetFile()->GetIndex(),
                  EDWORD, BrowseInfo->GetLineNo()->GetLineNumber(),
                  EWORD, BrowseInfo->GetCharPos(),
                  ESTRING, BrowseInfo->GetData(), NULL);
}
void ObjIeeeBinary::RenderExpression(ObjByte *buf, ObjExpression *Expression)
{
    switch(Expression->GetOp())
    {
        case ObjExpression::eNop:
            break;
        case ObjExpression::eNonExpression:
            RenderExpression(buf, Expression->GetLeft());
            RenderExpression(buf, Expression->GetRight());
            break;
        case ObjExpression::eValue:
            ContinueMessage(buf, embed('V'), EDWORD, Expression->GetValue(), NULL);
            break;
        case ObjExpression::eAdd:
            RenderExpression(buf, Expression->GetLeft());
            RenderExpression(buf, Expression->GetRight());
            ContinueMessage(buf, embed('+'), NULL);
            break;
        case ObjExpression::eSub:
            RenderExpression(buf, Expression->GetLeft());
            RenderExpression(buf, Expression->GetRight());
            ContinueMessage(buf, embed('-'), NULL);
            break;
        case ObjExpression::eMul:
            RenderExpression(buf, Expression->GetLeft());
            RenderExpression(buf, Expression->GetRight());
            ContinueMessage(buf, embed('*'), NULL);
            break;
        case ObjExpression::eDiv:
            RenderExpression(buf, Expression->GetLeft());
            RenderExpression(buf, Expression->GetRight());
            ContinueMessage(buf, embed('/'), NULL);
            break;
        case ObjExpression::eExpression:
            RenderExpression(buf, Expression->GetLeft()) ;
            break;
        case ObjExpression::eSymbol:
            if (Expression->GetSymbol()->GetType() == ObjSymbol::eExternal)
            {
                // externals get embedded in the expression
                ContinueMessage(buf, embed('X'), 
                                EDWORD, Expression->GetSymbol()->GetIndex(), NULL);
            }
            else
            {
                // other types of symbols we don't embed in the expression,
                // instead we embed their values
                RenderExpression(buf, Expression->GetSymbol()->GetOffset()) ;
            }
            break;
        case ObjExpression::eSection:
            ContinueMessage(buf, embed('R'), 
                            EDWORD, Expression->GetSection()->GetIndex(), NULL);
            break;
        case ObjExpression::ePC:
            ContinueMessage(buf, embed('P'),  NULL);
            break;
    }
}
void ObjIeeeBinary::GatherCS(const ObjByte *msg)
{
    int len = msg[1] + (msg[2] << 8);
    for (int i=0; i < len; i++)
        cs += msg[i];
}
bool ObjIeeeBinary::HandleWrite()
{
    ioBufferLen = 0;
    ioBuffer = new char [BUFFERSIZE];
    if (!ioBuffer)
        return false;
    ResetCS();
    WriteHeader();
    RenderCS();
    ResetCS();
    WriteFiles();
    RenderComment(eMakePass, ESTRING, ObjString("Make Pass Separator"), NULL);
    RenderCS();
    ResetCS();
    WriteSectionHeaders();	
    RenderCS();
    ResetCS();
    WriteTypes();	
    RenderCS();
    ResetCS();
    WriteSymbols();	
    WriteStartAddress();
    RenderCS();
    ResetCS();
    RenderComment(eLinkPass, ESTRING, ObjString("Link Pass Separator"), NULL);
    WriteSections();	
    RenderCS();
    ResetCS();
    RenderComment(eBrowsePass, ESTRING, ObjString("Browse Pass Separator"), NULL);
    WriteBrowseInfo();	
    RenderCS();
    ResetCS();
    WriteTrailer();
    flush();
    delete [] ioBuffer;
    ioBuffer = NULL;
    return true;
}
void ObjIeeeBinary::WriteHeader()
{
    RenderMessage(ecMB, ESTRING, translatorName, ESTRING, file->GetName(), NULL);
    RenderMessage(ecAD, EWORD, GetBitsPerMAU(), EWORD, GetMAUS(),
                  EBYTE, embed(GetFile()->GetBigEndian() ? 'M' : 'L'), NULL);
    RenderMessage(ecDT, ESTRING, ToTime(file->GetFileTime()), NULL);
    if (file->GetInputFile())
    {
        RenderFile(file->GetInputFile());
    }
    if (absolute)
    {
        RenderComment(eAbsolute, ESTRING, ObjString("Link Pass Separator"), NULL);
    }
}
void ObjIeeeBinary::WriteFiles()
{
    for (ObjFile::SourceFileIterator it = file->SourceFileBegin();
             it != file->SourceFileEnd(); ++it)
    {	
        RenderFile(*it);
    }
}
void ObjIeeeBinary::WriteSectionHeaders()
{
    for (ObjFile::SectionIterator it = file->SectionBegin();
             it != file->SectionEnd(); ++it)
    {	
        RenderSection(*it);
    }
}
void ObjIeeeBinary::WriteTypes()
{
    if (GetDebugInfoFlag())
    {
        for (ObjFile::TypeIterator it = file->TypeBegin();
             it != file->TypeEnd(); ++it)
        {
            RenderType(*it);
        }
    }	
}
void ObjIeeeBinary::WriteSymbols()
{
    for (ObjFile::SymbolIterator it = file->PublicBegin();
             it != file->PublicEnd(); ++it)
        RenderSymbol(*it);
    for (ObjFile::SymbolIterator it = file->ExternalBegin();
             it != file->ExternalEnd(); ++it)
        RenderSymbol(*it);
    for (ObjFile::SymbolIterator it = file->LocalBegin();
             it != file->LocalEnd(); ++it)
        RenderSymbol(*it);
    for (ObjFile::SymbolIterator it = file->AutoBegin();
             it != file->AutoEnd(); ++it)
        RenderSymbol(*it);
    for (ObjFile::SymbolIterator it = file->RegBegin();
             it != file->RegEnd(); ++it)
        RenderSymbol(*it);
    for (ObjFile::SymbolIterator it = file->DefinitionBegin();
             it != file->DefinitionEnd(); ++it)
        RenderSymbol(*it);
    for (ObjFile::SymbolIterator it = file->ImportBegin();
             it != file->ImportEnd(); ++it)
        RenderSymbol(*it);
    for (ObjFile::SymbolIterator it = file->ExportBegin();
             it != file->ExportEnd(); ++it)
        RenderSymbol(*it);
}
void ObjIeeeBinary::WriteStartAddress()
{
    if (startAddress)
    {
        RenderMessage(ecAS, embed('G'), embed('V'), EDWORD, startAddress, embed(0x1b), NULL);
    }
}
void ObjIeeeBinary::WriteSections()
{
    for (ObjFile::SectionIterator it = file->SectionBegin();
             it != file->SectionEnd(); ++it)
    {	
        RenderMessage(ecSB, EDWORD, (*it)->GetIndex(), NULL);
        RenderMemory(&(*it)->GetMemoryManager());
    }
}
void ObjIeeeBinary::WriteBrowseInfo()
{
    for (ObjFile::BrowseInfoIterator it = file->BrowseInfoBegin();
             it != file->BrowseInfoEnd(); ++it)
    {
        RenderBrowseInfo(*it);
    }	
}
void ObjIeeeBinary::WriteTrailer()
{
    RenderMessage(ecME, NULL);
}
void ObjIeeeBinary::RenderCS()
{
    // the CS is part of the checksum, but the number and '.' are not.
    RenderMessage(ecCS, EBYTE, cs + ecCS, NULL);
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
