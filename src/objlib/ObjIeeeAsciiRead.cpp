/*
    Software License Agreement (BSD License)
    parse
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
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

#define _CRT_SECURE_NO_WARNINGS  

#include "ObjFactory.h"
#include "ObjIeee.h"
#include <stdio.h>
#include <stack>
#include <ctype.h>
#include <stdio.h>
void DebugThrowHook()
{
}
ObjIeeeAscii::ParseData ObjIeeeAscii::parseData[] =
{
    ObjIeeeAscii::ParseData("LD", &ObjIeeeAscii::Data ),
    ObjIeeeAscii::ParseData("LR", &ObjIeeeAscii::Fixup ),
    ObjIeeeAscii::ParseData("LE", &ObjIeeeAscii::EnumeratedData ),
    ObjIeeeAscii::ParseData("NI", &ObjIeeeAscii::PublicSymbol ),
    ObjIeeeAscii::ParseData("NX", &ObjIeeeAscii::ExternalSymbol ),
    ObjIeeeAscii::ParseData("CO", &ObjIeeeAscii::Comment ),
    ObjIeeeAscii::ParseData("AT", &ObjIeeeAscii::TypeSpec),
    ObjIeeeAscii::ParseData("CS", &ObjIeeeAscii::CheckSum ),
    ObjIeeeAscii::ParseData("AS", &ObjIeeeAscii::GetOffset ),
    ObjIeeeAscii::ParseData("NN", &ObjIeeeAscii::LocalSymbol),
    ObjIeeeAscii::ParseData("NA", &ObjIeeeAscii::AutoSymbol ),
    ObjIeeeAscii::ParseData("NE", &ObjIeeeAscii::RegSymbol ),
    ObjIeeeAscii::ParseData("NT", &ObjIeeeAscii::TypeName ),
    ObjIeeeAscii::ParseData("SB", &ObjIeeeAscii::SectionDataHeader ),
    ObjIeeeAscii::ParseData("ST", &ObjIeeeAscii::SectionAttributes ),
    ObjIeeeAscii::ParseData("SA", &ObjIeeeAscii::SectionAlignment ),
    ObjIeeeAscii::ParseData("MB", &ObjIeeeAscii::ModuleStart ),
    ObjIeeeAscii::ParseData("DT", &ObjIeeeAscii::ModuleDate ),
    ObjIeeeAscii::ParseData("AD", &ObjIeeeAscii::ModuleAttributes ),
    ObjIeeeAscii::ParseData("ME", &ObjIeeeAscii::ModuleEnd ),
} ;
std::map<const char *, ObjIeeeAscii::ParseData *, ObjIeeeAscii::ParseDataLT> ObjIeeeAscii::parseTree;

ObjString ObjIeeeAscii::ParseString(const char *buffer, int *pos)
{
    int len = ObjUtil::FromHex(buffer, pos, 3);
    char name[512];
    if (len > strlen(buffer+*pos))
        ThrowSyntax(buffer, eAll);
    memcpy(name, buffer + *pos, len);
    name[len] = '\0';
    *pos += len;
    return ObjString (name);
}
void ObjIeeeAscii::ParseTime(const char *buffer, std::tm &tms, int *pos)
{
    tms.tm_year = ObjUtil::FromDecimal(buffer, pos, 4) - 1900;
    tms.tm_mon = ObjUtil::FromDecimal(buffer, pos, 2) - 1;
    tms.tm_mday = ObjUtil::FromDecimal(buffer, pos, 2);
    tms.tm_hour = ObjUtil::FromDecimal(buffer, pos, 2);
    tms.tm_min = ObjUtil::FromDecimal(buffer, pos, 2);
    tms.tm_sec = ObjUtil::FromDecimal(buffer, pos, 2);
}
ObjString ObjIeeeAscii::GetSymbolName(const char *buffer, int *index)
{
    int pos = 2;
    *index = ObjUtil::FromHex(buffer, &pos);
    if (buffer[pos++] != ',')
        ThrowSyntax(buffer, eAll);
    ObjString rv = ParseString(buffer, &pos);
    CheckTerm(buffer + pos);
    if (!GetCaseSensitiveFlag())
    {
        char buf[256];
        strncpy(buf, rv.c_str(), sizeof(buf));
        buf[sizeof(buf) - 1] = 0;
        char *p = buf;
        while (*p)
        {
            *p = toupper(*p);
            p++;
        }
        rv = buf;
    }
    return rv;
}
ObjSymbol *ObjIeeeAscii::FindSymbol(char ch, int index)
{
    ObjSymbol *sym = NULL;
    switch(ch)
    {
        case 'A':
            sym = GetSymbol(autos,index);
            break;
        case 'X':
            sym = GetSymbol(externals,index);
            break;
        case 'I':
            sym = GetSymbol(publics,index);
            break;
        case 'N':
            sym = GetSymbol(locals,index);
            break;
        case 'E':
            sym = GetSymbol(regs,index);
            break;
        default:
            break;
    }
    return sym;
}
ObjExpression *ObjIeeeAscii::GetExpression(const char *buffer, int *pos)
{
    std::stack<ObjExpression *> stack;
    bool done = false;
    while (!done)
    {
        char ch = buffer[(*pos)++];
        switch(ch)
        {
            case 'I':
            case 'N':
            case 'X':
            {
                int index = ObjUtil::FromHex(buffer, pos);
                ObjSymbol *sym = FindSymbol(ch, index);
                if (!sym)
                    ThrowSyntax(buffer, eAll);
                ObjExpression *exp = factory->MakeExpression(sym);
                stack.push(exp);
                break;
            }
            case 'R':
            {
                int index = ObjUtil::FromHex(buffer, pos);
                ObjSection *sect = GetSection(index);
                if (!sect)
                    ThrowSyntax(buffer, eAll);
                ObjExpression *exp = factory->MakeExpression(sect);
                stack.push(exp);
                break;
            }
            case 'P':
            {
                ObjExpression *exp = factory->MakeExpression(ObjExpression::ePC);
                stack.push(exp);
                break;
            }
            case '+':
            case '-':
            case '/':
            case '*':
            {
                ObjExpression::eOperator type;
                switch(ch)
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
                ObjExpression *right = stack.top(); stack.pop();
                ObjExpression *left = stack.top(); stack.pop();
                ObjExpression *exp = factory->MakeExpression(type, left, right);
                stack.push(exp);
                break;
            }
            default:
                (*pos)--;
                if (isxdigit(ch))
                {
                    int value = ObjUtil::FromHex(buffer, pos);
                    ObjExpression *exp = factory->MakeExpression(value);
                    stack.push(exp);
                }
                else
                    done = true;
        }
        if (buffer[*pos] != ',')
            done = true;
        else if (!done)
            (*pos)++;
    }
    ObjExpression *rv = NULL;
    if (stack.empty())
        ThrowSyntax(buffer, eAll);
    while (!stack.empty())
    {
        ObjExpression *exp = stack.top(); stack.pop();
        if (rv)
            rv = factory->MakeExpression(ObjExpression::eNonExpression, exp, rv);
        else
            rv = exp;
    }
    return rv;
}
bool ObjIeeeAscii::Parse(const char *buffer, eParseType ParseType)
{
    if (!parseTree.size())
    {
        ParseData *parser = &parseData[0];
        for (int i = 0; i < sizeof(parseData)/sizeof(parseData[0]); i++)
        {
            parseTree[parser->GetName()] = parser;
            parser++;
        }
    }
    std::map<const char *, ObjIeeeAscii::ParseData *, ObjIeeeAscii::ParseDataLT>::iterator it = parseTree.find(buffer);
    if (it != parseTree.end())
    {
        return it->second->Dispatch(this, buffer, ParseType);
    }
    else
        ObjIeeeAscii::ThrowSyntax(buffer, eAll);
    return true;
}
void ObjIeeeAscii::getline(char *buf, size_t size)
{
    // no point in optimizing this, it doesn't get any faster...
    fgets(buf, size, sfile);
    int l = strlen(buf);
    while (l && buf[l-1] < ' ')
        buf[--l] = 0;
}
ObjFile *ObjIeeeAscii::HandleRead(eParseType ParseType)
{
    bool done = false;
    ioBufferLen = 0;
    ioBufferPos = 0;
    ioBuffer = new char[BUFFERSIZE];
    if (!ioBuffer)
    {
        return NULL;
    }
    ResetCS();
    file = NULL;
    currentTags = new ObjMemory::DebugTagContainer;
    publics.clear();
    locals.clear();
    autos.clear();
    regs.clear();
    externals.clear();
    types.clear();
    sections.clear();
    files.clear();
    currentDataSection = NULL;
    lineno = 0;
    while (!done)
    {
        char inBuf[10000];
        lineno++;
        getline(inBuf,sizeof(inBuf));
           GatherCS(inBuf);
        try
        {
            done = Parse(inBuf, ParseType);
        }
        catch (BadCS &e)
        {
            if (ioBuffer)
                delete [] ioBuffer;
            ioBuffer = NULL;
            return NULL;
        }
        catch (SyntaxError &e)
        {
            if (ioBuffer)
                delete [] ioBuffer;
            ioBuffer = NULL;
            return NULL;
        }		
    }
    for (int i=0; i < publics.size(); i++)
        if (publics[i])
            file->Add(publics[i]);
    for (int i=0; i < locals.size(); i++)
        if (locals[i])
            file->Add(locals[i]);
    for (int i=0; i < autos.size(); i++)
        if (autos[i])
            file->Add(autos[i]);
    for (int i=0; i < regs.size(); i++)
        if (regs[i])
            file->Add(regs[i]);
    for (int i=0; i < externals.size(); i++)
        if (externals[i])
            file->Add(externals[i]);
    for (int i=1024; i < types.size(); i++)
        if (types[i])
            file->Add(types[i]);
    for (int i=0; i < sections.size(); i++)
        if (sections[i])
            file->Add(sections[i]);
    for (int i=0; i < files.size(); i++)
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
    delete currentTags;
    if (ioBuffer)
        delete [] ioBuffer;
    ioBuffer = NULL;
    return file;
}
bool ObjIeeeAscii::GetOffset(const char *buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int pos =3 ;
    char ch = buffer[2];
    int index = 0;
    if (ch != 'G')
        index = ObjUtil::FromHex(buffer, &pos);
    if (buffer[pos++] != ',')
        ThrowSyntax(buffer, ParseType);
    ObjExpression *exp = GetExpression(buffer, &pos);
    CheckTerm(buffer + pos);
    switch(ch)
    {
        case 'G':
            SetStartAddress(file, exp);
            break;
        case 'S':
        {
            ObjSection *sect = GetSection(index);
            if (!sect)
                ThrowSyntax(buffer, ParseType);
            sect->SetSize(exp);
            break;
        }
        case 'L':
        {
            ObjSection *sect = GetSection(index);
            if (!sect)
                ThrowSyntax(buffer, ParseType);
            sect->SetOffset(exp);
            break;
        }
        default:
        {
            ObjSymbol *sym = FindSymbol(ch, index);
            if (!sym)
                ThrowSyntax(buffer, ParseType);
            sym->SetOffset(exp);
            break;
        }
    }		
    return false;
}
void ObjIeeeAscii::DefinePointer(int index, const char *buffer, int *pos)
{
    ObjType *base;
    if (buffer[(*pos)++] != ',')
        ThrowSyntax(buffer, eAll);
    int sz = ObjUtil::FromHex(buffer, pos);
    if (buffer[(*pos)++] != ',')
        ThrowSyntax(buffer, eAll);
    if (buffer[(*pos)++] != 'T')
        ThrowSyntax(buffer, eAll);
    int old = ObjUtil::FromHex(buffer, pos);
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
     
    ObjType *type = GetType(index);
    if (type)
    {
        if (type->GetType() != ObjType::eNone)
            ThrowSyntax(buffer, eAll);
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
void ObjIeeeAscii::DefineBitField(int index, const char *buffer, int *pos)
{
//		ATT$,T3,sz,TB,bitno,bits.
    if (buffer[(*pos)++] != ',')
        ThrowSyntax(buffer, eAll);
    int sz = ObjUtil::FromHex(buffer, pos);
    if (buffer[(*pos)++] != ',')
        ThrowSyntax(buffer, eAll);
    if (buffer[(*pos)++] != 'T')
        ThrowSyntax(buffer, eAll);
    int old = ObjUtil::FromHex(buffer, pos);
    if (buffer[(*pos)++] != ',')
        ThrowSyntax(buffer, eAll);
    int startBit = ObjUtil::FromHex(buffer, pos);
    if (buffer[(*pos)++] != ',')
        ThrowSyntax(buffer, eAll);
    int bitCount = ObjUtil::FromHex(buffer, pos);
    ObjType *base;
    if (old <= ObjType::eReservedTop)
        base = factory->MakeType((ObjType::eType)old);
    else
        base = GetType(old);
    if (!base)
    {
        base = factory->MakeType(ObjType::eNone, old);
        PutType(old, base);
    }
    ObjType *type = GetType(index);
    if (type)
    {
        if (type->GetType() != ObjType::eNone)
            ThrowSyntax(buffer, eAll);
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
void ObjIeeeAscii::DefineTypeDef(int index, const char *buffer, int *pos)
{
    ObjType *base;
    if (buffer[(*pos)++] != ',')
        ThrowSyntax(buffer, eAll);
    if (buffer[(*pos)++] != 'T')
        ThrowSyntax(buffer, eAll);
    int old = ObjUtil::FromHex(buffer, pos);
    if (old <= ObjType::eReservedTop)
        base = factory->MakeType((ObjType::eType)old);
    else
        base = GetType(old);
    if (!base)
    {
        base = factory->MakeType(ObjType::eNone, old);
        PutType(old, base);
    }
    ObjType *type = GetType(index);
    if (type)
    {
        if (type->GetType() != ObjType::eNone)
            ThrowSyntax(buffer, eAll);
        type->SetType(ObjType::eTypeDef);
        type->SetBaseType(base);
    }
    else
    {
        type = factory->MakeType(ObjType::eTypeDef, base, index);
        PutType(index, type);
    }
}
void ObjIeeeAscii::DefineArray(ObjType::eType definer, int index, const char *buffer, int *pos)
{
    if (buffer[(*pos)++] != ',')
        ThrowSyntax(buffer, eAll);
    int sz = ObjUtil::FromHex(buffer, pos);
    if (buffer[(*pos)++] != ',')
        ThrowSyntax(buffer, eAll);
    if (buffer[(*pos)++] != 'T')
        ThrowSyntax(buffer, eAll);
    int baseIndex = ObjUtil::FromHex(buffer, pos);

    ObjType *baseType;
    if (baseIndex <= ObjType::eReservedTop)
        baseType = factory->MakeType((ObjType::eType)baseIndex);
    else
         baseType = GetType(baseIndex);
    if (!baseType)
    {
        baseType = factory->MakeType(ObjType::eNone, index);
        PutType(baseIndex, baseType);
    }
        
    if (buffer[(*pos)++] != ',')
        ThrowSyntax(buffer, eAll);
    if (buffer[(*pos)++] != 'T')
        ThrowSyntax(buffer, eAll);
    int indexIndex = ObjUtil::FromHex(buffer, pos);

    ObjType *indexType ;
    if (indexIndex <= ObjType::eReservedTop)
        indexType = factory->MakeType((ObjType::eType)indexIndex);
    else
        indexType = GetType(indexIndex);
    if (!indexType)
    {
        indexType = factory->MakeType(ObjType::eNone, index);
        PutType(indexIndex, indexType);
    }
        
    int base=0, top=0;	
    if (definer == ObjType::eArray)
    {
        if (buffer[(*pos)++] != ',')
            ThrowSyntax(buffer, eAll);
        base = ObjUtil::FromHex(buffer, pos);
    
        if (buffer[(*pos)++] != ',')
            ThrowSyntax(buffer, eAll);
        top = ObjUtil::FromHex(buffer, pos);
    }

    ObjType *type = GetType(index);
    if (type)
    {
        if (type->GetType() != ObjType::eNone)
            ThrowSyntax(buffer, eAll);
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
void ObjIeeeAscii::DefineFunction(int index, const char *buffer, int *pos)
{
    if (buffer[(*pos)++] != ',')
        ThrowSyntax(buffer, eAll);
    if (buffer[(*pos)++] != 'T')
        ThrowSyntax(buffer, eAll);
    int rvIndex = ObjUtil::FromHex(buffer, pos);
    
    ObjType *rvType ;
    if (rvIndex <= ObjType::eReservedTop)
        rvType = factory->MakeType((ObjType::eType)rvIndex);
    else
        rvType = GetType(rvIndex);
    if (!rvType)
    {
        rvType = factory->MakeType(ObjType::eNone, rvIndex);
        PutType(rvIndex, rvType);
    }

    if (buffer[(*pos)++] != ',')
        ThrowSyntax(buffer, eAll);
    ObjFunction::eLinkage linkage = (ObjFunction::eLinkage)ObjUtil::FromHex(buffer, pos);
    ObjString name("");
    
    ObjType *type = GetType(index);
    if (type)
    {
        // we are going to retype the original type, so get the spec'd name
        name = type->GetName();
    }
    ObjFunction *function = factory->MakeFunction(name, rvType, index);
    function->SetLinkage(linkage);
    while (buffer[(*pos)] == ',')
    {
        (*pos)++;
        if (buffer[(*pos)++] != 'T')
            ThrowSyntax(buffer, eAll);
        int argIndex = ObjUtil::FromHex(buffer, pos);
        ObjType *argType;
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
void ObjIeeeAscii::DefineStruct(ObjType::eType stype, int index, const char *buffer, int *pos)
{
    ObjType *type = GetType(index);
    if (!type)
    {
        type = factory->MakeType(stype, index);
        PutType(index, type);
    }
    else
    {
        if (type->GetType() != ObjType::eNone)
            ThrowSyntax(buffer, eAll);
        type->SetType(stype);
        type->SetIndex(index);
    }
    if (stype != ObjType::eField)
    {
        if (buffer[(*pos)++] != ',')
            ThrowSyntax(buffer, eAll);
        type->SetSize(ObjUtil::FromHex(buffer, pos));
    }
    while (buffer[*pos] == ',')
    {
        int val;
        ObjString fieldName;
        (*pos)++;
        if (buffer[(*pos)++] != 'T')
            ThrowSyntax(buffer, eAll);
        int fieldTypeIndex = ObjUtil::FromHex(buffer, pos);
        ObjType *fieldTypeBase ;
        if (fieldTypeIndex <= ObjType::eReservedTop)
            fieldTypeBase = factory->MakeType((ObjType::eType)fieldTypeIndex, 0);
        else
            fieldTypeBase = GetType(fieldTypeIndex);
        if (!fieldTypeBase)
        {
            fieldTypeBase = factory->MakeType((ObjType::eType)ObjType::eNone, 0);
            PutType(fieldTypeIndex, fieldTypeBase);
        }
                
        if (!fieldTypeBase)
            ThrowSyntax(buffer, eAll);
        // chaining to next field definition?
        if (fieldTypeBase->GetType() == ObjType::eField)
        {
            for (ObjType::FieldIterator it = fieldTypeBase->FieldBegin();
                     it != fieldTypeBase->FieldEnd(); ++it)
                type->Add(*it);
            break;
        }
        else
        {
            if (buffer[(*pos)++] != ',')
                ThrowSyntax(buffer, eAll);
            fieldName= ParseString(buffer, pos);
            if (buffer[(*pos)++] != ',')
                ThrowSyntax(buffer, eAll);
            val = ObjUtil::FromHex(buffer, pos);
            ObjField *field = factory->MakeField(fieldName, fieldTypeBase, val, index);
            type->Add(field);			
        }
    }
}
void ObjIeeeAscii::DefineType(int index, const char *buffer, int *pos)
{
    if (buffer[(*pos)++] != 'T')
        ThrowSyntax(buffer, eAll);
    int definer = ObjUtil::FromHex(buffer, pos);
    if (definer >= (int)ObjType::eVoid || index <= (int)ObjType::eReservedTop)
        ThrowSyntax(buffer, eAll);
    switch((ObjType::eType)definer)
    {
        case ObjType::ePointer:
            DefinePointer(index, buffer, pos);
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
bool ObjIeeeAscii::TypeName(const char *buffer, eParseType parseType)
{
    if (!file)
        ThrowSyntax(buffer, parseType);
    int pos = 2;
    int index = ObjUtil::FromHex(buffer, &pos);
    if (buffer[pos++] != ',')
        ThrowSyntax(buffer, parseType);
    ObjString name = ParseString(buffer, &pos);
    CheckTerm(buffer + pos);
    ObjType *type = GetType(index);
    if (!type)
    {
        type = factory->MakeType(ObjType::eNone, index);
        PutType(index, type);
    }
    type->SetName(name);
    return false;	
}
bool ObjIeeeAscii::TypeSpec(const char *buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int pos = 2;
    char ch = buffer[pos++];
    int index = ObjUtil::FromHex(buffer, &pos);
    if (buffer[pos++] != ',')
        ThrowSyntax(buffer, ParseType);
    switch (ch)
    {
        case 'A':
        case 'E':
        case 'X':
        case 'N':
        case 'I':
        {
            ObjSymbol *symbol = FindSymbol(ch, index);
            if (!symbol)
                ThrowSyntax(buffer, ParseType);
            if (buffer[pos++] != 'T')
                ThrowSyntax(buffer, ParseType);
            index = ObjUtil::FromHex(buffer, &pos);
            CheckTerm(buffer + pos);
            ObjType *type;
            /* won't get a 'special' type deriving type here */
            if (index < ObjType::eReservedTop)
                type = factory->MakeType((ObjType::eType)index);
            else
                type = GetType(index);
            if (!type)
                ThrowSyntax(buffer, ParseType);
            symbol->SetBaseType(type);	
            break;
        }
        case 'T':
        {
            DefineType(index, buffer, &pos);
            CheckTerm(buffer + pos);
            break;
        }
        default:
            ThrowSyntax(buffer, ParseType);
            break;
    }
    return false;
}
bool ObjIeeeAscii::Comment(const char *buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int pos = 2;
    int cnum = ObjUtil::FromDecimal(buffer, &pos,3);
    if (buffer[pos++] != ',')
        ThrowSyntax(buffer, ParseType);
    ObjString contents = ParseString(buffer, &pos);
    const char *data = contents.c_str();
    CheckTerm(buffer + pos);
    switch (cnum)
    {
        case eAbsolute:
            absolute = true; // absolute file
            break;
        case eMakePass:
            if (ParseType == eMake)
                return true;
            break;
        case eLinkPass:
            if (ParseType == eLink)
                return true;
            break;
        case eBrowsePass:
            if (ParseType == eBrowse)
                return true;
            break;
        case eConfig: /* not supported */
            break;
        case eDefinition:
        {
            int pos = 0;
            ObjString name = ParseString(data, &pos);
            if (data[pos++] != ',')
                ThrowSyntax(buffer, ParseType);
            ObjInt value = ObjUtil::FromDecimal(data, &pos);
            ObjDefinitionSymbol *sym = factory->MakeDefinitionSymbol(name);	
            sym->SetValue(value);
            file->Add(sym);
            break;
        }
        case eExport:
        {
            bool byOrdinal;
            if (data[0] == 'O')
                byOrdinal = true;
            else if (data[0] == 'N')
                byOrdinal = false;
            else 
                ThrowSyntax(buffer, ParseType);
            if (data[1] != ',')
                ThrowSyntax(buffer, ParseType);
            int pos = 2;
            ObjString name = ParseString(data, &pos);
            if (data[pos++] != ',')
                ThrowSyntax(buffer, ParseType);
            ObjString externalName;
            int ordinal = 0xffffffff;
            if (byOrdinal)
                ordinal = ObjUtil::FromDecimal(data, &pos);
            else
                externalName = ParseString(data, &pos);
            ObjString moduleName;
            if (data[pos] == ',')
            {
                pos++;
                moduleName = ParseString(data, &pos);
            }
            if (!GetCaseSensitiveFlag())
            {
                for (int i=0; i < name.size(); i++)
                    name[i] = toupper(name[i]);
            }
            ObjExportSymbol *sym = factory->MakeExportSymbol(name);	
            sym->SetByOrdinal(byOrdinal);
            sym->SetOrdinal(ordinal);
            sym->SetExternalName(externalName);
            sym->SetDllName(moduleName);
            file->Add(sym);
            break;
        }
        case eImport:
        {
            bool byOrdinal;
            if (data[0] == 'O')
                byOrdinal = true;
            else if (data[0] == 'N')
                byOrdinal = false;
            else 
                ThrowSyntax(buffer, ParseType);
            if (data[1] != ',')
                ThrowSyntax(buffer, ParseType);
            int pos = 2;
            ObjString name = ParseString(data, &pos);
            if (data[pos++] != ',')
                ThrowSyntax(buffer, ParseType);
            ObjString externalName;
            int ordinal = 0xffffffff;
            ObjString dllName;
            if (byOrdinal)
                ordinal = ObjUtil::FromDecimal(data, &pos);
            else
                externalName = ParseString(data, &pos);
            if (data[pos++] != ',')
                ThrowSyntax(buffer, ParseType);
            dllName = ParseString(data, &pos);
            if (!GetCaseSensitiveFlag())
            {
                for (int i=0; i < name.size(); i++)
                    name[i] = toupper(name[i]);
            }
            ObjImportSymbol *sym = factory->MakeImportSymbol(name);	
            sym->SetByOrdinal(byOrdinal);
            sym->SetOrdinal(ordinal);
            sym->SetExternalName(externalName);
            sym->SetDllName(dllName);
            file->Add(sym);
            break;
        }
        case eSourceFile:
        {
            int pos = 0;
            int index = ObjUtil::FromDecimal(data, &pos);
            if (data[pos++] != ',')
                ThrowSyntax(buffer, ParseType);
            ObjString name = ParseString(data, &pos);
            if (data[pos++] != ',')
                ThrowSyntax(buffer, ParseType);
            std::tm time;
            ParseTime(data, time, &pos);
            ObjSourceFile *sf = factory->MakeSourceFile(name, index);
            sf->SetFileTime(time);
            PutFile(index, sf);
            break;
        }
        case eBrowseInfo:
        {
            ObjBrowseInfo::eType type;
            ObjBrowseInfo::eQual qual;
            int pos = 0;
            type = (ObjBrowseInfo::eType)ObjUtil::FromHex(data, &pos);
            if (data[pos++] != ',')
                ThrowSyntax(buffer, ParseType);
            qual = (ObjBrowseInfo::eQual)ObjUtil::FromHex(data, &pos);
            if (data[pos++] != ',')
                ThrowSyntax(buffer, ParseType);
            int filenum = ObjUtil::FromDecimal(data, &pos);
            if (data[pos++] != ',')
                ThrowSyntax(buffer, ParseType);
            int lineno = ObjUtil::FromDecimal(data, &pos);
            if (data[pos++] != ',')
                ThrowSyntax(buffer, ParseType);
            int charpos = ObjUtil::FromDecimal(data, &pos);
            if (data[pos++] != ',')
                ThrowSyntax(buffer, ParseType);
            ObjString extra = ParseString(data, &pos);
            ObjSourceFile *sf = files[filenum];
            if (!sf)
                ThrowSyntax(buffer, ParseType);
            ObjLineNo *line = factory->MakeLineNo(sf, lineno);
            ObjBrowseInfo *bi = factory->MakeBrowseInfo(type, qual, line, charpos, extra);	
            file->Add(bi);
            break;
        }
        case eLineNo:
        {
            int pos = 0;
            int index = ObjUtil::FromDecimal(data, &pos);
            if (data[pos++] != ',')
                ThrowSyntax(buffer, ParseType);
            int line = ObjUtil::FromDecimal(data, &pos);
            ObjSourceFile *file = files[index];
            if (!file)
                ThrowSyntax(buffer, ParseType);
            ObjLineNo *lineNo= factory->MakeLineNo(file, line);
            ObjDebugTag *tag = factory->MakeDebugTag(lineNo);
            currentTags->push_back(tag);
            break;
        }	
        case eVar:
        {
            int pos = 0;
            int ch = data[pos++];
            int index = ObjUtil::FromHex(data, &pos);
            ObjSymbol *sym = FindSymbol(ch, index);
            if (!sym)
                ThrowSyntax(buffer, ParseType);
            ObjDebugTag *tag = factory->MakeDebugTag(sym);
            currentTags->push_back(tag);
            break;
        }
        case eBlockStart:
        {
            ObjDebugTag *tag = factory->MakeDebugTag(true);
            currentTags->push_back(tag);
            break;
        }
        case eBlockEnd:
        {
            ObjDebugTag *tag = factory->MakeDebugTag(false);
            currentTags->push_back(tag);
            break;
        }
        case eFunctionStart:
        case eFunctionEnd:
            int pos = 0;
            int ch = data[pos++];
            int index = ObjUtil::FromHex(data, &pos);
            ObjSymbol *sym = FindSymbol(ch, index);
            if (!sym)
                ThrowSyntax(buffer, ParseType);
            ObjDebugTag *tag = factory->MakeDebugTag(sym, cnum == eFunctionStart);
            currentTags->push_back(tag);
            break;
    }
    return false;
}
bool ObjIeeeAscii::PublicSymbol(const char *buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int index = 0;
    ObjString name = GetSymbolName(buffer, &index);
    ObjSymbol *sym = factory->MakePublicSymbol(name, index);
    PutSymbol(publics, index, sym);
    sym->SetSourceFile(GetFile(0));
    return false;
}
bool ObjIeeeAscii::LocalSymbol(const char *buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int index = 0;
    ObjString name = GetSymbolName(buffer, &index);
    ObjSymbol *sym = factory->MakeLocalSymbol(name, index);
    PutSymbol(locals, index, sym);
    sym->SetSourceFile(GetFile(0));
    return false;
}
bool ObjIeeeAscii::AutoSymbol(const char *buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int index = 0;
    ObjString name = GetSymbolName(buffer, &index);
    
    ObjSymbol *sym = GetSymbol(autos,index);
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
bool ObjIeeeAscii::RegSymbol(const char *buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int index = 0;
    ObjString name = GetSymbolName(buffer, &index);
    ObjSymbol *sym = factory->MakeRegSymbol(name, index);
    PutSymbol(regs, index, sym);
    return false;
}
bool ObjIeeeAscii::ExternalSymbol(const char *buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int index = 0;
    ObjString name = GetSymbolName(buffer, &index);
    ObjSymbol *sym = factory->MakeExternalSymbol(name, index);
    PutSymbol(externals, index, sym);
    return false;
}
bool ObjIeeeAscii::SectionAttributes(const char *buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int pos = 2;
    int index = ObjUtil::FromHex(buffer, &pos);
    if (buffer[pos++] != ',')
        ThrowSyntax(buffer, ParseType);
    ObjInt quals = 0;
    ObjString name;
    while (buffer[pos] && buffer[pos+1] == ',')
    {
        switch (buffer[pos++])
        {
            case 'A':
                quals |= ObjSection::absolute;
                break;
            case 'B':
                quals |= ObjSection::bit;
                break;
            case 'C':
                quals |= ObjSection::common;
                break;
            case 'E':
                quals |= ObjSection::equal;
                break;
            case 'M':
                quals |= ObjSection::max;
                break;
            case 'N':
                quals |= ObjSection::now;
                break;
            case 'P':
                quals |= ObjSection::postpone;
                break;
            case 'R':
                quals |= ObjSection::rom;
                break;
            case 'S':
                quals |= ObjSection::separate;
                break;
            case 'U':
                quals |= ObjSection::unique;
                break;
            case 'V':
                quals |= ObjSection::virt;
                break;
            case 'W':
                quals |= ObjSection::ram;
                break;
            case 'x':
                quals |= ObjSection::exec;
                break;
            case 'Z':
                quals |= ObjSection::zero;
                break;
            default:
		        ThrowSyntax(buffer, ParseType);
                break;
        }
        pos++;
    }
    name = ParseString(buffer, &pos);
    CheckTerm(buffer + pos);
    ObjSection *sect = factory->MakeSection(name, index);
    sect->SetQuals(quals);
    PutSection(index, sect);
    return false;
}
bool ObjIeeeAscii::SectionAlignment(const char *buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int pos = 2;
    int index = ObjUtil::FromHex(buffer, &pos);
    if (buffer[pos++] != ',')
        ThrowSyntax(buffer, ParseType);
    int align = ObjUtil::FromHex(buffer, &pos);
    CheckTerm(buffer + pos);
    ObjSection *sect = GetSection(index);
    if (!sect)
        ThrowSyntax(buffer, ParseType);
    sect->SetAlignment(align);
    return false;
}
bool ObjIeeeAscii::SectionDataHeader(const char *buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int pos = 2;
    int index = ObjUtil::FromHex(buffer, &pos);
    CheckTerm(buffer + pos);
    ObjSection *sect = GetSection(index);
    if (!sect)
        ThrowSyntax(buffer, ParseType);
    currentDataSection = sect;
    return false;
}
bool ObjIeeeAscii::Data(const char *buffer, eParseType ParseType)
{
    // this function is optimized to not use C++ stream objects
    // because it is called a lot, and the resultant memory allocations
    // really slow down linker and librarian operations
    if (!file || currentDataSection == NULL)
        ThrowSyntax(buffer, ParseType);
    ObjByte data[1024]; 
    int pos = 2, i=0;
    while (isxdigit(buffer[pos]))
    {
        int n = buffer[pos++] - '0';
        if (n > 9)
            n -= 7 ;
        int m = buffer[pos++] - '0';
        if (m > 9)
            m -= 7;
        if (i >= 1024)
            ThrowSyntax(buffer, ParseType);
        data[i++] = (n << 4) + m;
    }
    CheckTerm(buffer + pos);
    ObjMemory *mem = factory->MakeData(data, i);
    mem->SetDebugTags(currentTags);
    currentTags = new ObjMemory::DebugTagContainer;
    currentDataSection->Add(mem);
    return false;
}
bool ObjIeeeAscii::EnumeratedData(const char *buffer, eParseType ParseType)
{
    if (!file || currentDataSection == NULL)
        ThrowSyntax(buffer, ParseType);
    int pos = 2;
    if (buffer[pos++] != '(')
        ThrowSyntax(buffer, ParseType);
    ObjInt size = ObjUtil::FromHex(buffer, &pos);
    if (buffer[pos++] != ',')
        ThrowSyntax(buffer, ParseType);
    ObjByte fill = ObjUtil::FromHex(buffer, &pos);
    if (buffer[pos++] != ')')
        ThrowSyntax(buffer, ParseType);
    CheckTerm(buffer + pos);
    ObjMemory *mem = factory->MakeData(size, fill);
    mem->SetDebugTags(currentTags);
    currentTags = new ObjMemory::DebugTagContainer;
    currentDataSection->Add(mem);
    return false;
}
bool ObjIeeeAscii::Fixup(const char *buffer, eParseType ParseType)
{
    if (!file || currentDataSection == NULL)
        ThrowSyntax(buffer, ParseType);
    int pos = 3;
    if (buffer[2] != '(')
        ThrowSyntax(buffer, ParseType);
    ObjExpression *exp = GetExpression(buffer, &pos);
    if (buffer[pos++] != ')')
        ThrowSyntax(buffer, ParseType);
    CheckTerm(buffer + pos);
    if (exp->GetOperator() != ObjExpression::eNonExpression)
        ThrowSyntax(buffer, ParseType);
    if (exp->GetRight()->GetOperator() != ObjExpression::eValue)
        ThrowSyntax(buffer, ParseType);
    int size = exp->GetRight()->GetValue();
    ObjExpression *left = exp->GetLeft();
    ObjMemory *mem = factory->MakeFixup(left, size);
    mem->SetDebugTags(currentTags);
    currentTags = new ObjMemory::DebugTagContainer;
    currentDataSection->Add(mem);
    return false;
}
bool ObjIeeeAscii::ModuleStart(const char *buffer, eParseType ParseType)
{
    if (file)
        ThrowSyntax(buffer, ParseType);
    char translator[256];
    int i = 2;
    for (i=2; buffer[i] && buffer[i] != ','; i++)
        translator[i-2] = buffer[i];
    translator[i-2] = '\0';
    if (buffer[i++] != ',')
        ThrowSyntax(buffer, ParseType);
    
    ObjString fileName = ParseString(buffer, &i);
    CheckTerm(buffer + i);
    
    file = factory->MakeFile(fileName);
    SetTranslatorName(std::string(translator));
    return false;
}
bool ObjIeeeAscii::ModuleEnd(const char *buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    CheckTerm(buffer + 2);
    return true;
}
bool ObjIeeeAscii::ModuleAttributes(const char *buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    int pos = 2;
    int bitsPerMau = ObjUtil::FromDecimal(buffer, &pos);
    if (buffer[pos++] != ',')
        ThrowSyntax(buffer, ParseType);
    int maus = ObjUtil::FromDecimal(buffer, &pos);
    if (buffer[pos++] != ',')
        ThrowSyntax(buffer, ParseType);
    bool bigEndian = buffer[pos] == 'M';
    if (buffer[pos] != 'M' && buffer[pos] != 'L')
        ThrowSyntax(buffer, ParseType);
    pos++;
    CheckTerm(buffer + pos);
    file->SetBigEndian(bigEndian);
    SetMAUS(maus);
    SetBitsPerMAU(bitsPerMau);
    return false;
}
bool ObjIeeeAscii::ModuleDate(const char *buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    std::tm time;
    int pos = 2;
    ParseTime(buffer, time, &pos);
    CheckTerm(buffer + pos);
    file->SetFileTime(time);
    return false;
}
bool ObjIeeeAscii::CheckSum(const char *buffer, eParseType ParseType)
{
    if (!file)
        ThrowSyntax(buffer, ParseType);
    CheckTerm(buffer + 4);
    cs -= buffer[2] + buffer[3] + buffer[4];
    cs &= 127;
    int pos = 2;
    int readcs = ObjUtil::FromHex(buffer, &pos, 2);
    if (readcs != cs)
    {
        BadCS e;
        throw e;
    }
    ResetCS();
    
    return false;
}
void ObjIeeeAscii::PutSymbol(SymbolMap &map, int index, ObjSymbol *sym)
{
    if (map.size() <= index)
    {
        map.resize(index > 100 ? index*2 : 200);
    }
    map[index] = sym;
}
void ObjIeeeAscii::PutType(int index, ObjType *type)
{
    if (types.size() <= index)
    {
        types.resize(index > 100 ? index*2 : 200);
    }
    types[index] = type;
}
void ObjIeeeAscii::PutSection(int index, ObjSection *sect)
{
    if (sections.size() <= index)
    {
        sections.resize(index > 100 ? index*2 : 200);
    }
    sections[index] = sect;
}
void ObjIeeeAscii::PutFile(int index, ObjSourceFile *file)
{
    if (files.size() <= index)
    {
        files.resize(index > 100 ? index*2 : 200);
    }
    files[index] = file;
}
