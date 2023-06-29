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
#include <stack>
#include <cctype>
#include <cstdio>
#include <cassert>

ObjString ObjIeeeBinary::ParseString(const ObjByte* buffer, int* pos)
{
    int len = buffer[(*pos)++] << 8;
    len += buffer[(*pos)++];
    char name[4096];
    memcpy(name, buffer + *pos, len);
    name[len] = '\0';
    *pos += len;
    return name;
}
void ObjIeeeBinary::ParseTime(const ObjByte* buffer, std::tm& tms, int* pos)
{
    ObjString str = ParseString(buffer, pos);
    const char* p = str.c_str();
    int xx = 0;
    memset(&tms, 0, sizeof(tms));
    tms.tm_year = ObjUtil::FromDecimal(p, &xx, 4) - 1900;
    tms.tm_mon = ObjUtil::FromDecimal(p, &xx, 2) - 1;
    tms.tm_mday = ObjUtil::FromDecimal(p, &xx, 2);
    tms.tm_hour = ObjUtil::FromDecimal(p, &xx, 2);
    tms.tm_min = ObjUtil::FromDecimal(p, &xx, 2);
    tms.tm_sec = ObjUtil::FromDecimal(p, &xx, 2);
}
ObjString ObjIeeeBinary::GetSymbolName(const ObjByte* buffer, int* index)
{
    int pos = 3 + 1;
    *index = GetIndex(buffer, &pos);
    ObjString rv = ParseString(buffer, &pos);
    CheckTerm(buffer, pos);
    if (!GetCaseSensitiveFlag())
    {
        char buf[256];
        strncpy(buf, rv.c_str(), sizeof(buf));
        buf[sizeof(buf) - 1] = 0;
        char* p = buf;
        while (*p)
        {
            *p = toupper(*p);
            p++;
        }
        rv = buf;
    }
    return rv;
}
ObjSymbol* ObjIeeeBinary::FindSymbol(char ch, int index)
{
    ObjSymbol* sym = nullptr;
    switch (ch)
    {
        case 'A':
            sym = GetSymbol(autos, index);
            break;
        case 'X':
            sym = GetSymbol(externals, index);
            break;
        case 'I':
            sym = GetSymbol(publics, index);
            break;
        case 'N':
            sym = GetSymbol(locals, index);
            break;
        case 'E':
            sym = GetSymbol(regs, index);
            break;
        default:
            break;
    }
    return sym;
}
ObjExpression* ObjIeeeBinary::GetExpression(const ObjByte* buffer, int* pos)
{
    std::stack<ObjExpression*> stack;
    bool done = false;
    while (!done)
    {
        char ch = buffer[(*pos)++];
        switch (ch)
        {
            case 'I':
            case 'N':
            case 'X': {
                int index = GetIndex(buffer, pos);
                ObjSymbol* sym = FindSymbol(ch, index);
                if (!sym)
                    ThrowSyntax(buffer, eAll);
                ObjExpression* exp = factory->MakeExpression(sym);
                stack.push(exp);
                break;
            }
            case 'R': {
                int index = GetIndex(buffer, pos);
                ObjSection* sect = GetSection(index);
                if (!sect)
                    ThrowSyntax(buffer, eAll);
                ObjExpression* exp = factory->MakeExpression(sect);
                stack.push(exp);
                break;
            }
            case 'P': {
                ObjExpression* exp = factory->MakeExpression(ObjExpression::ePC);
                stack.push(exp);
                break;
            }
            case '+':
            case '-':
            case '/':
            case '*': {
                ObjExpression::eOperator type;
                switch (ch)
                {
                    case '+':
                        type = ObjExpression::eAdd;
                        break;
                    case '-':
                        type = ObjExpression::eSub;
                        break;
                    case '/':
                        type = ObjExpression::eDiv;
                        break;
                    case '*':
                        type = ObjExpression::eMul;
                        break;
                    default:
                        ThrowSyntax(buffer, eAll);
                        break;
                }
                if (stack.size() < 2)
                    ThrowSyntax(buffer, eAll);
                ObjExpression* right = stack.top();
                stack.pop();
                ObjExpression* left = stack.top();
                stack.pop();
                ObjExpression* exp = factory->MakeExpression(type, left, right);
                stack.push(exp);
                break;
            }
            case '\x1b':  // end of expression
                done = true;
                break;
            case 'V': {
                int value = GetDWord(buffer, pos);
                ObjExpression* exp = factory->MakeExpression(value);
                stack.push(exp);
                break;
            }
            default:
                //                assert(0);
                break;
        }
    }
    ObjExpression* rv = nullptr;
    if (stack.empty())
        ThrowSyntax(buffer, eAll);
    while (!stack.empty())
    {
        ObjExpression* exp = stack.top();
        stack.pop();
        if (rv)
            rv = factory->MakeExpression(ObjExpression::eNonExpression, exp, rv);
        else
            rv = exp;
    }
    return rv;
}
bool ObjIeeeBinary::Parse(const ObjByte* buffer, eParseType ParseType)
{
    switch (buffer[0])
    {
        case 0:
            ThrowSyntax(buffer, eAll);
        case ecLD:
            return Data(buffer, ParseType);
        case ecLR:
            return Fixup(buffer, ParseType);
        case ecLE:
            return EnumeratedData(buffer, ParseType);
        case ecNAME:
            switch (buffer[3])
            {
                case 'I':
                    return PublicSymbol(buffer, ParseType);
                case 'X':
                    return ExternalSymbol(buffer, ParseType);
                case 'N':
                    return LocalSymbol(buffer, ParseType);
                case 'A':
                    return AutoSymbol(buffer, ParseType);
                case 'R':
                    return RegSymbol(buffer, ParseType);
                case 'T':
                    return TypeName(buffer, ParseType);
                default:
                    ThrowSyntax(buffer, eAll);
            }
        case ecCO:
            return Comment(buffer, ParseType);
        case ecAT:
            return TypeSpec(buffer, ParseType);
        case ecCS:
            return CheckSum(buffer, ParseType);
        case ecAS:
            return GetOffset(buffer, ParseType);
        case ecSB:
            return SectionDataHeader(buffer, ParseType);
        case ecST:
            return SectionAttributes(buffer, ParseType);
        case ecSA:
            return SectionAlignment(buffer, ParseType);
        case ecMB:
            return ModuleStart(buffer, ParseType);
        case ecDT:
            return ModuleDate(buffer, ParseType);
        case ecAD:
            return ModuleAttributes(buffer, ParseType);
        case ecME:
            return ModuleEnd(buffer, ParseType);
        default:
            ThrowSyntax(buffer, eAll);
    }
}
void ObjIeeeBinary::getline(ObjByte* buf, size_t size)
{
    if (!fread(buf, 1, 3, sfile))
    {
        memset(buf, 0, 3);
        return;
    }
    int len = (buf[1] << 8) + buf[2];
    if (len > BUFFERSIZE)
        ThrowSyntax(buf, eAll);
    if (fread(buf + 3, 1, len - 3, sfile) != len - 3)
        ThrowSyntax(buf, eAll);
}
ObjFile* ObjIeeeBinary::HandleRead(eParseType ParseType)
{
    bool done = false;
    ioBufferLen = 0;
    ioBufferPos = 0;
    ioBuffer = std::make_unique<char[]>(BUFFERSIZE);
    ResetCS();
    file = nullptr;
    currentTags = std::make_unique<ObjMemory::DebugTagContainer>();
    publics.clear();
    locals.clear();
    autos.clear();
    regs.clear();
    externals.clear();
    types.clear();
    sections.clear();
    files.clear();
    currentDataSection = nullptr;
    while (!done)
    {
        ObjByte inBuf[10000];
        getline(inBuf, sizeof(inBuf));
        GatherCS(inBuf);
        try
        {
            done = Parse(inBuf, ParseType);
        }
        catch (BadCS& e)
        {
            ioBuffer = nullptr;
            return nullptr;
        }
        catch (SyntaxError& e)
        {
            ioBuffer = nullptr;
            return nullptr;
        }
    }
    for (int i = 0; i < publics.size(); i++)
        if (publics[i])
            file->Add(publics[i]);
    for (int i = 0; i < locals.size(); i++)
        if (locals[i])
            file->Add(locals[i]);
    for (int i = 0; i < autos.size(); i++)
        if (autos[i])
            file->Add(autos[i]);
    for (int i = 0; i < regs.size(); i++)
        if (regs[i])
            file->Add(regs[i]);
    for (int i = 0; i < externals.size(); i++)
        if (externals[i])
            file->Add(externals[i]);
    for (int i = 1024; i < types.size(); i++)
        if (types[i])
            file->Add(types[i]);
    for (int i = 0; i < sections.size(); i++)
        if (sections[i])
            file->Add(sections[i]);
    for (int i = 0; i < files.size(); i++)
        if (files[i])
            file->Add(files[i]);
    publics.clear();
    locals.clear();
    autos.clear();
    regs.clear();
    externals.clear();
    types.clear();
    sections.clear();
    files.clear();
    factory->GetIndexManager()->LoadIndexes(file);
    currentTags.release();
    ioBuffer = std::make_unique<char[]>(BUFFERSIZE);
    ioBuffer = nullptr;
    return file;
}
bool ObjIeeeBinary::GetOffset(const ObjByte* buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int pos = 3;
    char ch = GetByte(buffer, &pos);
    int index = 0;
    if (ch != 'G')
        index = GetIndex(buffer, &pos);
    ObjExpression* exp = GetExpression(buffer, &pos);
    CheckTerm(buffer, pos);
    switch (ch)
    {
        case 'G':
            SetStartAddress(file, exp);
            break;
        case 'S': {
            ObjSection* sect = GetSection(index);
            if (!sect)
                ThrowSyntax(buffer, ParseType);
            sect->SetSize(exp);
            break;
        }
        case 'L': {
            ObjSection* sect = GetSection(index);
            if (!sect)
                ThrowSyntax(buffer, ParseType);
            sect->SetOffset(exp);
            break;
        }
        default: {
            ObjSymbol* sym = FindSymbol(ch, index);
            if (!sym)
                ThrowSyntax(buffer, ParseType);
            sym->SetOffset(exp);
            break;
        }
    }
    return false;
}
void ObjIeeeBinary::DefinePointer(ObjType::eType, int index, const ObjByte* buffer, int* pos)
{
    ObjType* base;
    int sz = GetDWord(buffer, pos);
    int old = GetIndex(buffer, pos);
    /* shouldn't get 'special' types here */
    if (old <= ObjType::eReservedTop)
        base = factory->MakeType((ObjType::eType)old);
    else
        base = GetType(old);
    if (!base)
    {
        base = factory->MakeType(ObjType::eNone, old);
        PutType(old, base);
    }

    ObjType* type = GetType(index);
    if (type)
    {
        if (type->GetType() != ObjType::eNone)
            ThrowSyntax(buffer, eAll);
        type->SetIndex(index);
        type->SetType(ObjType::ePointer);
        type->SetBaseType(base);
    }
    else
    {
        type = factory->MakeType(ObjType::ePointer, base, index);
        PutType(index, type);
    }
    type->SetSize(sz);
}
void ObjIeeeBinary::DefineBitField(int index, const ObjByte* buffer, int* pos)
{
    //		ATT$,T3,sz,TB,bitno,bits.
    int sz = GetByte(buffer, pos);
    int old = GetIndex(buffer, pos);
    int startBit = GetByte(buffer, pos);
    int bitCount = GetByte(buffer, pos);
    ObjType* base;
    if (old <= ObjType::eReservedTop)
        base = factory->MakeType((ObjType::eType)old);
    else
        base = GetType(old);
    if (!base)
    {
        base = factory->MakeType(ObjType::eNone, old);
        PutType(old, base);
    }
    ObjType* type = GetType(index);
    if (type)
    {
        if (type->GetType() != ObjType::eNone)
            ThrowSyntax(buffer, eAll);
        type->SetIndex(index);
        type->SetType(ObjType::eBitField);
        type->SetBaseType(base);
    }
    else
    {
        type = factory->MakeType(ObjType::eBitField, base, index);
        PutType(index, type);
    }
    type->SetSize(sz);
    type->SetStartBit(startBit);
    type->SetBitCount(bitCount);
}
void ObjIeeeBinary::DefineTypeDef(int index, const ObjByte* buffer, int* pos)
{
    ObjType* base;
    int old = GetIndex(buffer, pos);
    if (old <= ObjType::eReservedTop)
        base = factory->MakeType((ObjType::eType)old);
    else
        base = GetType(old);
    if (!base)
    {
        base = factory->MakeType(ObjType::eNone, old);
        PutType(old, base);
    }
    ObjType* type = GetType(index);
    if (type)
    {
        if (type->GetType() != ObjType::eNone)
            ThrowSyntax(buffer, eAll);
        type->SetIndex(index);
        type->SetType(ObjType::eTypeDef);
        type->SetBaseType(base);
    }
    else
    {
        type = factory->MakeType(ObjType::eTypeDef, base, index);
        PutType(index, type);
    }
}
void ObjIeeeBinary::DefineArray(ObjType::eType definer, int index, const ObjByte* buffer, int* pos)
{
    int sz = GetDWord(buffer, pos);
    int baseIndex = GetIndex(buffer, pos);

    ObjType* baseType;
    if (baseIndex <= ObjType::eReservedTop)
        baseType = factory->MakeType((ObjType::eType)baseIndex);
    else
        baseType = GetType(baseIndex);
    if (!baseType)
    {
        baseType = factory->MakeType(ObjType::eNone, index);
        PutType(baseIndex, baseType);
    }

    int indexIndex = GetIndex(buffer, pos);

    ObjType* indexType;
    if (indexIndex <= ObjType::eReservedTop)
        indexType = factory->MakeType((ObjType::eType)indexIndex);
    else
        indexType = GetType(indexIndex);
    if (!indexType)
    {
        indexType = factory->MakeType(ObjType::eNone, index);
        PutType(indexIndex, indexType);
    }

    int base = 0, top = 0;
    if (definer == ObjType::eArray)
    {
        base = GetDWord(buffer, pos);
        top = GetDWord(buffer, pos);
    }

    ObjType* type = GetType(index);
    if (type)
    {
        if (type->GetType() != ObjType::eNone)
            ThrowSyntax(buffer, eAll);
        type->SetIndex(index);
        type->SetType(definer);
        type->SetBaseType(baseType);
    }
    else
    {
        type = factory->MakeType(definer, baseType, index);
        PutType(index, type);
    }
    type->SetIndexType(indexType);
    type->SetBase(base);
    type->SetTop(top);
    type->SetSize(sz);
}
void ObjIeeeBinary::DefineFunction(int index, const ObjByte* buffer, int* pos)
{
    int rvIndex = GetIndex(buffer, pos);

    ObjType* rvType;
    if (rvIndex <= ObjType::eReservedTop)
        rvType = factory->MakeType((ObjType::eType)rvIndex);
    else
        rvType = GetType(rvIndex);
    if (!rvType)
    {
        rvType = factory->MakeType(ObjType::eNone, rvIndex);
        PutType(rvIndex, rvType);
    }
    ObjFunction::eLinkage linkage = (ObjFunction::eLinkage)GetDWord(buffer, pos);
    ObjString name("");

    ObjType* type = GetType(index);
    if (type)
    {
        // we are going to retype the original type, so get the spec'd name
        name = type->GetName();
    }
    ObjFunction* function = factory->MakeFunction(name, rvType, index);
    function->SetLinkage(linkage);
    int len = (buffer[1] << 8) + buffer[2];
    while (*pos < len)
    {
        int argIndex = GetIndex(buffer, pos);
        ObjType* argType;
        if (argIndex <= ObjType::eReservedTop)
            argType = factory->MakeType((ObjType::eType)argIndex);
        else
            argType = GetType(argIndex);
        if (!argType)
        {
            argType = factory->MakeType(ObjType::eNone, argIndex);
            PutType(argIndex, argType);
        }
        function->Add(argType);
    }
    PutType(index, function);
}
void ObjIeeeBinary::DefineStruct(ObjType::eType stype, int index, const ObjByte* buffer, int* pos)
{
    ObjType* type = GetType(index);
    if (!type)
    {
        type = factory->MakeType(stype, index);
        PutType(index, type);
    }
    else
    {
        if (type->GetType() != ObjType::eNone)
            ThrowSyntax(buffer, eAll);
        type->SetIndex(index);
        type->SetType(stype);
        type->SetIndex(index);
    }
    if (stype != ObjType::eField)
    {
        type->SetSize(GetDWord(buffer, pos));
    }
    int len = (buffer[1] << 8) + buffer[2];
    while (*pos < len)
    {
        int val;
        ObjString fieldName;
        int fieldTypeIndex = GetIndex(buffer, pos);
        ObjType* fieldTypeBase;
        if (fieldTypeIndex <= ObjType::eReservedTop)
            fieldTypeBase = factory->MakeType((ObjType::eType)fieldTypeIndex, 0);
        else
            fieldTypeBase = GetType(fieldTypeIndex);
        if (!fieldTypeBase)
        {
            fieldTypeBase = factory->MakeType((ObjType::eType)ObjType::eNone, fieldTypeIndex);
            PutType(fieldTypeIndex, fieldTypeBase);
        }
        else
        {
            fieldTypeBase->SetIndex(fieldTypeIndex);
        }

        if (!fieldTypeBase)
            ThrowSyntax(buffer, eAll);
        // chaining to next field definition?
        if (fieldTypeBase->GetType() == ObjType::eField)
        {
            for (ObjType::FieldIterator it = fieldTypeBase->FieldBegin(); it != fieldTypeBase->FieldEnd(); ++it)
                type->Add(*it);
            break;
        }
        else
        {
            fieldName = ParseString(buffer, pos);
            val = GetDWord(buffer, pos);
            ObjField* field = factory->MakeField(fieldName, fieldTypeBase, val, index);
            type->Add(field);
        }
    }
}
void ObjIeeeBinary::DefineType(int index, const ObjByte* buffer, int* pos)
{
    int definer = GetIndex(buffer, pos);
    if (definer >= (int)ObjType::eVoid || index <= (int)ObjType::eReservedTop)
        ThrowSyntax(buffer, eAll);
    switch ((ObjType::eType)definer)
    {
        case ObjType::ePointer:
            DefinePointer(ObjType::ePointer, index, buffer, pos);
            break;
        case ObjType::eBitField:
            DefineBitField(index, buffer, pos);
            break;
        case ObjType::eTypeDef:
            DefineTypeDef(index, buffer, pos);
            break;
        case ObjType::eArray:
        case ObjType::eVla:
            DefineArray((ObjType::eType)definer, index, buffer, pos);
            break;
        case ObjType::eFunction:
            DefineFunction(index, buffer, pos);
            break;
        case ObjType::eStruct:
        case ObjType::eUnion:
        case ObjType::eEnum:
        case ObjType::eField:
            DefineStruct((ObjType::eType)definer, index, buffer, pos);
            break;
        default:
            ThrowSyntax(buffer, eAll);
            break;
    }
}
bool ObjIeeeBinary::TypeName(const ObjByte* buffer, eParseType parseType)
{
    if (!file)
        ThrowSyntax(buffer, parseType);
    int pos = 4;
    int index = GetIndex(buffer, &pos);
    ObjString name = ParseString(buffer, &pos);
    CheckTerm(buffer, pos);
    ObjType* type = GetType(index);
    if (!type)
    {
        type = factory->MakeType(ObjType::eNone, index);
        PutType(index, type);
    }
    type->SetName(name);
    return false;
}
bool ObjIeeeBinary::TypeSpec(const ObjByte* buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int pos = 3;
    char ch = GetByte(buffer, &pos);
    int index = GetIndex(buffer, &pos);
    switch (ch)
    {
        case 'A':
        case 'E':
        case 'X':
        case 'N':
        case 'I': {
            ObjSymbol* symbol = FindSymbol(ch, index);
            index = GetIndex(buffer, &pos);
            CheckTerm(buffer, pos);
            ObjType* type;
            /* won't get a 'special' type deriving type here */
            if (index < ObjType::eReservedTop)
                type = factory->MakeType((ObjType::eType)index);
            else
                type = GetType(index);
            if (!type)
                ThrowSyntax(buffer, ParseType);
            else if (symbol)
                symbol->SetBaseType(type);
            break;
        }
        case 'T': {
            DefineType(index, buffer, &pos);
            CheckTerm(buffer, pos);
            break;
        }
        case 'R': {
            ObjSection* sect = GetSection(index);
            if (!sect)
            {
                ThrowSyntax(buffer, eAll);
            }
            index = GetIndex(buffer, &pos);
            CheckTerm(buffer, pos);
            ObjType* type;
            /* won't get a 'special' type deriving type here */
            if (index < ObjType::eReservedTop)
                type = factory->MakeType((ObjType::eType)index);
            else
                type = GetType(index);
            if (!type)
            {
                ThrowSyntax(buffer, ParseType);
            }
            sect->SetVirtualType(type);
        }
        break;
        default:
            ThrowSyntax(buffer, ParseType);
            break;
    }
    return false;
}
bool ObjIeeeBinary::Comment(const ObjByte* buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int pos = 3;
    int cnum = GetWord(buffer, &pos);
    switch (cnum)
    {
        case eAbsolute:
            absolute = true;  // absolute file
            ParseString(buffer, &pos);
            break;
        case eMakePass:
            if (ParseType == eMake)
                return true;
            ParseString(buffer, &pos);
            break;
        case eLinkPass:
            if (ParseType == eLink)
                return true;
            ParseString(buffer, &pos);
            break;
        case eBrowsePass:
            if (currentTags && currentDataSection)
            {
                currentDataSection->Add(std::move(currentTags));
                currentTags = std::make_unique<ObjMemory::DebugTagContainer>();
            }
            if (ParseType == eBrowse)
                return true;
            ParseString(buffer, &pos);
            break;
        case eConfig: /* not supported */
            GetDWord(buffer, &pos);
            break;
        case eDefinition: {
            ObjString name = ParseString(buffer, &pos);
            ObjInt value = GetDWord(buffer, &pos);
            ObjDefinitionSymbol* sym = factory->MakeDefinitionSymbol(name);
            sym->SetValue(value);
            file->Add(sym);
            break;
        }
        case eExport: {
            bool byOrdinal;
            int ch = GetByte(buffer, &pos);
            if (ch == 'O')
                byOrdinal = true;
            else if (ch == 'N')
                byOrdinal = false;
            else
                ThrowSyntax(buffer, ParseType);
            ObjString name = ParseString(buffer, &pos);
            ObjString externalName;
            int ordinal = 0xffffffff;
            if (byOrdinal)
                ordinal = GetDWord(buffer, &pos);
            else
                externalName = ParseString(buffer, &pos);
            ObjString moduleName;
            int len = (buffer[1] << 8) + buffer[2];
            if (pos < len)
            {
                moduleName = ParseString(buffer, &pos);
            }
            if (!GetCaseSensitiveFlag())
            {
                for (int i = 0; i < name.size(); i++)
                    name[i] = toupper(name[i]);
            }
            ObjExportSymbol* sym = factory->MakeExportSymbol(name);
            sym->SetByOrdinal(byOrdinal);
            sym->SetOrdinal(ordinal);
            sym->SetExternalName(externalName);
            sym->SetDllName(moduleName);
            file->Add(sym);
            break;
        }
        case eImport: {
            bool byOrdinal;
            int ch = GetByte(buffer, &pos);
            if (ch == 'O')
                byOrdinal = true;
            else if (ch == 'N')
                byOrdinal = false;
            else
                ThrowSyntax(buffer, ParseType);
            ObjString name = ParseString(buffer, &pos);
            ObjString externalName;
            int ordinal = 0xffffffff;
            ObjString dllName;
            if (byOrdinal)
                ordinal = GetDWord(buffer, &pos);
            else
                externalName = ParseString(buffer, &pos);
            dllName = ParseString(buffer, &pos);
            if (!GetCaseSensitiveFlag())
            {
                for (int i = 0; i < name.size(); i++)
                    name[i] = toupper(name[i]);
            }
            ObjImportSymbol* sym = factory->MakeImportSymbol(name);
            sym->SetByOrdinal(byOrdinal);
            sym->SetOrdinal(ordinal);
            sym->SetExternalName(externalName);
            sym->SetDllName(dllName);
            file->Add(sym);
            break;
        }
        case eSourceFile: {
            int index = GetIndex(buffer, &pos);
            ObjString name = ParseString(buffer, &pos);
            std::tm time;
            ParseTime(buffer, time, &pos);
            ObjSourceFile* sf = factory->MakeSourceFile(name, index);
            sf->SetFileTime(time);
            PutFile(index, sf);
            break;
        }
        case eBrowseInfo: {
            ObjBrowseInfo::eType type;
            ObjBrowseInfo::eQual qual;
            type = (ObjBrowseInfo::eType)GetIndex(buffer, &pos);
            qual = (ObjBrowseInfo::eQual)GetDWord(buffer, &pos);
            int filenum = GetDWord(buffer, &pos);
            int lineno = GetDWord(buffer, &pos);
            int charpos = GetWord(buffer, &pos);
            ObjString extra = ParseString(buffer, &pos);
            ObjSourceFile* sf = files[filenum];
            if (!sf)
                ThrowSyntax(buffer, ParseType);
            ObjLineNo* line = factory->MakeLineNo(sf, lineno);
            ObjBrowseInfo* bi = factory->MakeBrowseInfo(type, qual, line, charpos, extra);
            file->Add(bi);
            break;
        }
        case eLineNo: {
            int index = GetIndex(buffer, &pos);
            int line = GetDWord(buffer, &pos);
            ObjSourceFile* file = files[index];
            if (!file)
                ThrowSyntax(buffer, ParseType);
            ObjLineNo* lineNo = factory->MakeLineNo(file, line);
            ObjDebugTag* tag = factory->MakeDebugTag(lineNo);
            currentTags->push_back(tag);
            break;
        }
        case eVar: {
            int ch = GetByte(buffer, &pos);
            int index = GetIndex(buffer, &pos);
            ObjSymbol* sym = FindSymbol(ch, index);
            if (!sym)
                ThrowSyntax(buffer, ParseType);
            ObjDebugTag* tag = factory->MakeDebugTag(sym);
            currentTags->push_back(tag);
            break;
        }
        case eBlockStart: {
            ObjDebugTag* tag = factory->MakeDebugTag(true);
            currentTags->push_back(tag);
            ParseString(buffer, &pos);
            break;
        }
        case eBlockEnd: {
            ObjDebugTag* tag = factory->MakeDebugTag(false);
            currentTags->push_back(tag);
            ParseString(buffer, &pos);
            break;
        }
        case eFunctionStart:
        case eFunctionEnd:
            int ch = GetByte(buffer, &pos);
            int index = GetIndex(buffer, &pos);
            if (ch == 'R')
            {
                ObjSection* sect = GetSection(index);
                if (!sect)
                    ThrowSyntax(buffer, ParseType);
                ObjDebugTag* tag = factory->MakeDebugTag(sect, cnum == eFunctionStart);
                currentTags->push_back(tag);
            }
            else
            {
                ObjSymbol* sym = FindSymbol(ch, index);
                if (!sym)
                    ThrowSyntax(buffer, ParseType);
                ObjDebugTag* tag = factory->MakeDebugTag(sym, cnum == eFunctionStart);
                currentTags->push_back(tag);
            }
            break;
    }
    CheckTerm(buffer, pos);
    return false;
}
bool ObjIeeeBinary::PublicSymbol(const ObjByte* buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int index = 0;
    ObjString name = GetSymbolName(buffer, &index);
    ObjSymbol* sym = factory->MakePublicSymbol(name, index);
    PutSymbol(publics, index, sym);
    sym->SetSourceFile(GetFile(0));
    return false;
}
bool ObjIeeeBinary::LocalSymbol(const ObjByte* buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int index = 0;
    ObjString name = GetSymbolName(buffer, &index);
    ObjSymbol* sym = factory->MakeLocalSymbol(name, index);
    PutSymbol(locals, index, sym);
    sym->SetSourceFile(GetFile(0));
    return false;
}
bool ObjIeeeBinary::AutoSymbol(const ObjByte* buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int index = 0;
    ObjString name = GetSymbolName(buffer, &index);

    ObjSymbol* sym = GetSymbol(autos, index);
    // autos can be forward declared in function type declarations
    if (sym)
    {
        sym->SetName(name);
    }
    else
    {
        sym = factory->MakeAutoSymbol(name, index);
        PutSymbol(autos, index, sym);
    }
    return false;
}
bool ObjIeeeBinary::RegSymbol(const ObjByte* buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int index = 0;
    ObjString name = GetSymbolName(buffer, &index);
    ObjSymbol* sym = factory->MakeRegSymbol(name, index);
    PutSymbol(regs, index, sym);
    return false;
}
bool ObjIeeeBinary::ExternalSymbol(const ObjByte* buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int index = 0;
    ObjString name = GetSymbolName(buffer, &index);
    ObjSymbol* sym = factory->MakeExternalSymbol(name, index);
    PutSymbol(externals, index, sym);
    return false;
}
bool ObjIeeeBinary::SectionAttributes(const ObjByte* buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int pos = 3;
    int index = GetIndex(buffer, &pos);
    ObjInt quals = GetDWord(buffer, &pos);
    ObjString name = ParseString(buffer, &pos);
    CheckTerm(buffer, pos);
    ObjSection* sect = factory->MakeSection(name, index);
    sect->SetQuals(quals);
    PutSection(index, sect);
    return false;
}
bool ObjIeeeBinary::SectionAlignment(const ObjByte* buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int pos = 3;
    int index = GetIndex(buffer, &pos);
    int align = GetDWord(buffer, &pos);
    CheckTerm(buffer, pos);
    ObjSection* sect = GetSection(index);
    if (!sect)
        ThrowSyntax(buffer, ParseType);
    sect->SetAlignment(align);
    return false;
}
bool ObjIeeeBinary::SectionDataHeader(const ObjByte* buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int pos = 3;
    int index = GetIndex(buffer, &pos);
    CheckTerm(buffer, pos);
    ObjSection* sect = GetSection(index);
    if (!sect)
        ThrowSyntax(buffer, ParseType);
    if (currentTags && currentDataSection)
    {
        currentDataSection->Add(std::move(currentTags));
        currentTags = std::make_unique<ObjMemory::DebugTagContainer>();
    }
    currentDataSection = sect;
    return false;
}
bool ObjIeeeBinary::Data(const ObjByte* buffer, eParseType ParseType)
{
    if (!file || currentDataSection == nullptr)
        ThrowSyntax(buffer, ParseType);
    int n = (buffer[1] << 8) + buffer[2];
    ObjMemory* mem = factory->MakeData(const_cast<ObjByte*>(buffer) + 3, n - 3);
    // no need to check the termination as we consume everything */
    mem->SetDebugTags(std::move(currentTags));
    currentTags = std::make_unique<ObjMemory::DebugTagContainer>();
    currentDataSection->Add(mem);
    return false;
}
bool ObjIeeeBinary::EnumeratedData(const ObjByte* buffer, eParseType ParseType)
{
    if (!file || currentDataSection == nullptr)
        ThrowSyntax(buffer, ParseType);
    int pos = 3;
    ObjInt size = GetDWord(buffer, &pos);
    ObjByte fill = GetByte(buffer, &pos);
    CheckTerm(buffer, pos);
    ObjMemory* mem = factory->MakeData(size, fill);
    mem->SetDebugTags(std::move(currentTags));
    currentTags = std::make_unique<ObjMemory::DebugTagContainer>();
    currentDataSection->Add(mem);
    return false;
}
bool ObjIeeeBinary::Fixup(const ObjByte* buffer, eParseType ParseType)
{
    if (!file || currentDataSection == nullptr)
        ThrowSyntax(buffer, ParseType);
    int pos = 3;
    ObjExpression* exp = GetExpression(buffer, &pos);
    int size = GetByte(buffer, &pos);
    CheckTerm(buffer, pos);

    ObjMemory* mem = factory->MakeFixup(exp, size);
    mem->SetDebugTags(std::move(currentTags));
    currentTags = std::make_unique<ObjMemory::DebugTagContainer>();
    currentDataSection->Add(mem);
    return false;
}
bool ObjIeeeBinary::ModuleStart(const ObjByte* buffer, eParseType ParseType)
{
    if (file)
        ThrowSyntax(buffer, ParseType);
    int pos = 3;
    ObjString translator = ParseString(buffer, &pos);

    ObjString fileName = ParseString(buffer, &pos);
    CheckTerm(buffer, pos);

    file = factory->MakeFile(fileName);
    SetTranslatorName(std::string(translator));
    return false;
}
bool ObjIeeeBinary::ModuleEnd(const ObjByte* buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    CheckTerm(buffer, 3);
    return true;
}
bool ObjIeeeBinary::ModuleAttributes(const ObjByte* buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int pos = 3;
    int bitsPerMau = GetByte(buffer, &pos);
    int maus = GetByte(buffer, &pos);
    int ch = GetByte(buffer, &pos);
    bool bigEndian = ch == 'M';
    if (ch != 'M' && ch != 'L')
        ThrowSyntax(buffer, ParseType);
    CheckTerm(buffer, pos);
    file->SetBigEndian(bigEndian);
    SetMAUS(maus);
    SetBitsPerMAU(bitsPerMau);
    return false;
}
bool ObjIeeeBinary::ModuleDate(const ObjByte* buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    std::tm time;
    int pos = 3;
    ParseTime(buffer, time, &pos);
    CheckTerm(buffer, pos);
    file->SetFileTime(time);
    return false;
}
bool ObjIeeeBinary::CheckSum(const ObjByte* buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    cs -= buffer[1] + buffer[2] + buffer[3];
    cs &= 0xff;
    int pos = 3;
    int readcs = GetByte(buffer, &pos);
    if (readcs != cs)
    {
        BadCS e;
        throw e;
    }
    CheckTerm(buffer, pos);
    ResetCS();

    return false;
}
void ObjIeeeBinary::PutSymbol(SymbolMap& map, int index, ObjSymbol* sym)
{
    if (map.size() <= index)
    {
        map.resize(index > 100 ? index * 2 : 200);
    }
    map[index] = sym;
}
void ObjIeeeBinary::PutType(int index, ObjType* type)
{
    if (types.size() <= index)
    {
        types.resize(index > 100 ? index * 2 : 200);
    }
    types[index] = type;
}
void ObjIeeeBinary::PutSection(int index, ObjSection* sect)
{
    if (sections.size() <= index)
    {
        sections.resize(index > 100 ? index * 2 : 200);
    }
    sections[index] = sect;
}
void ObjIeeeBinary::PutFile(int index, ObjSourceFile* file)
{
    if (files.size() <= index)
    {
        files.resize(index > 100 ? index * 2 : 200);
    }
    files[index] = file;
}
