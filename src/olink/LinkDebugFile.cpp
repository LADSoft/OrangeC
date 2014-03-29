/*
    Software License Agreement (BSD License)
    
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
#include "ObjFile.h"
#include "ObjSourceFile.h"
#include "ObjSection.h"
#include "ObjMemory.h"
#include "ObjDebugTag.h"
#include "ObjLineNo.h"
#include "ObjFunction.h"
#include "ObjSymbol.h"
#include "ObjExpression.h"
#include "ObjUtil.h"
#include "LinkDebugFile.h"
#include "Utils.h"
#include "LinkDebugAux.h"
#include <stdio.h>
#include <deque>
#include <set>
#include <ctype.h>
#include <fstream>

#define STRINGVERSION "100"
#define DBVersion atoi(STRINGVERSION)

static int version_ok;

// well I didn't originally use virtual tables.  It turns out that virtual tables
// speed things up for individual tables, but when you are writing as database the 
// disk writes are the limiting factor.  Nonetheless I remain with the virtual table 
// approach because the codelooks much prettier...
char *LinkDebugFile::pragmas=
{
    "PRAGMA journal_mode = MEMORY; PRAGMA synchronous = OFF;PRAGMA temp_store = MEMORY;"
};
char *LinkDebugFile::tables= 
{
    "BEGIN; "
    "CREATE TABLE dbPropertyBag ("
    " property VARCHAR(100)"
    " ,value VARCHAR(200)"
    " );"
    "CREATE TABLE FileNames ("
    " id INTEGER PRIMARY KEY"
    " ,name VARCHAR(260)"
    " );"
    "CREATE TABLE LineNumbers ("
    " address INTEGER PRIMARY KEY"
    " ,fileId INTEGER"
    " ,line INTEGER"
//    " ,FOREIGN KEY (fileId) REFERENCES FileNames(id)"
    " );"
    "CREATE TABLE LineNumbersFull("
    " fileID INTEGER"
    " ,alias INTEGER"
    " ,line INTEGER"
    " );"
    "CREATE TABLE Types ("
    " id INTEGER PRIMARY KEY" // groups types
    " ,qualifier INTEGER"
    " ,size INTEGER"
    " ,baseType INTEGER"
    " ,indexType INTEGER"
    " ,arrayBase INTEGER"
    " ,arrayTop INTEGER"
    " ,startBit INTEGER"
    " ,bitCount INTEGER"
    " ,returnType INTEGER"
    " ,name VARCHAR(200)"
    " );"
    "CREATE TABLE Names ("
    " id INTEGER PRIMARY KEY AUTOINCREMENT"
    " ,name VARCHAR(200)"
    " );"
    "CREATE TABLE TypeNames ("
    " symbolId INTEGER"
    " ,typeId INTEGER"
//    " ,FOREIGN KEY (typeId) REFERENCES Types(id)"
//    " ,FOREIGN KEY (symbolId) REFERENCES Names(id)"
    " );"
    "CREATE TABLE Fields ("
    " typeId INTEGER" // groups types                          
    " ,baseType INTEGER"
    " ,symbolId INTEGER"
    " ,offsetOrOrd INTEGER"
    " ,fieldOrder INTEGER"
//    " ,FOREIGN KEY (typeId) REFERENCES Types(id)"
//    " ,FOREIGN KEY (symbolId) REFERENCES Names(id)"
    " );"
    "CREATE TABLE Args ("
    " typeId INTEGER"
    " ,paramId INTEGER"
    " ,argOrder INTEGER"
//    " ,FOREIGN KEY (typeId) REFERENCES Types(id)"
    " );"    
    "CREATE TABLE Globals ("
    "  symbolId INTEGER"
    " ,typeId INTEGER"
    " ,varAddress INTEGER"
    " ,fileId INTEGER"
//    " ,lineNo INTEGER"  // future
//    " ,FOREIGN KEY (symbolId) REFERENCES Names(id)"
//    " ,FOREIGN KEY (typeId) REFERENCES Types(id)"
    " );"
    "CREATE TABLE Locals ("
    "  symbolId INTEGER"
    " ,typeId INTEGER"
    " ,varAddress INTEGER"
    " ,fileId INTEGER"
//    " ,lineNo INTEGER"  // future
//    " ,FOREIGN KEY (symbolId) REFERENCES Names(id)"
//    " ,FOREIGN KEY (typeId) REFERENCES Types(id)"
    " );"
    "CREATE TABLE Autos ("
    "  symbolId INTEGER"
    " ,typeId INTEGER"
    " ,funcId INTEGER"
    " ,fileId INTEGER"
    " ,varOffset INTEGER"
    " ,beginLine INTEGER"
    " ,endLine INTEGER"
//    " ,FOREIGN KEY (symbolId) REFERENCES Names(id)"
//    " ,FOREIGN KEY (fileId) REFERENCES FileNames(id)"
    " );"
    "INSERT INTO dbPropertyBag (property, value)"
    " VALUES (\"dbVersion\", "STRINGVERSION");"
    "COMMIT; "
} ;
char *LinkDebugFile::indexes = 
{
    "CREATE INDEX LNIndex ON LineNumbers(fileid,line);"
    "CREATE INDEX TNIndex1 ON TypeNames(symbolId);"
    "CREATE INDEX TNIndex2 ON TypeNames(typeId);"
    "CREATE INDEX FDIndex ON Fields(typeId);"
    "CREATE INDEX ARIndex ON Args(typeId);"
    "CREATE INDEX GBLIndex1 ON Globals(symbolId, fileId);"
    "CREATE INDEX GBLIndex2 ON Globals(varAddress, fileId);"
    "CREATE INDEX LCLIndex ON Locals(symbolId, fileId);"
    "CREATE INDEX AUTIndex ON Autos(symbolId, fileId, funcId);"
} ;
LinkDebugFile::~LinkDebugFile()
{
}
bool LinkDebugFile::Begin(void)
{
    bool rv = true; 
    if (!SQLiteExec("BEGIN"))
    {
        rv = false;
    }
    return rv;
}
bool LinkDebugFile::End(void )
{
    bool rv = true;
    if (!SQLiteExec("END"))
    {
        rv = false;
    }
    return rv;
}
bool LinkDebugFile::SQLiteExec( char *str)
{
    char *zErrMsg  = 0;
    bool rv = false;
    int rc = sqlite3_exec(dbPointer, str, 0, 0, &zErrMsg);
    if( rc!=SQLITE_OK )
    {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
        rv = true;
    }
    return rv;
}
bool LinkDebugFile::CreateTables(void)
{
    bool rv = true;
    if (!SQLiteExec(pragmas))
    {
        rv = false;
    }
    if (!SQLiteExec(tables))
    {
        rv = false;
    }
    return rv;
}
bool LinkDebugFile::CreateIndexes(void)
{
    bool rv = true;
    if (!SQLiteExec(indexes))
    {
        rv = false;
    }
    return rv;
}
bool LinkDebugFile::DBOpen(char *name)
{
    bool rv = false;
    dbPointer = NULL;
    unlink(name);
    if (sqlite3_open_v2(name, &dbPointer, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL) == SQLITE_OK)
    {
        rv = CreateTables();
    }
    else
    {
        // we don't want to reuse a database for a variety of reasons...
        rv = false;
    }
    if (rv)
        sqlite3_busy_timeout(dbPointer, 100);
    return rv;
}
bool LinkDebugFile::WriteFileNames()
{
    std::vector<sqlite3_int64> v;
    std::vector<ObjString *> n;
    for (ObjFile::SourceFileIterator it = file->SourceFileBegin(); it != file->SourceFileEnd(); ++it)
    {
        ObjString name = (*it)->GetName();
        int index = (*it)->GetIndex();
        char lcname[260];
        int i;
        for (i=0; i < name.size(); ++i)
            lcname[i] = tolower(name[i]);
        lcname[i] = 0;
        v.push_back(index);
        n.push_back(new ObjString(lcname));
    }
    LinkerColumnsWithNameVirtualTable fns(v, n, 2, true);
    fns.Start(dbPointer);
    fns.InsertIntoFrom("filenames");
    fns.Stop();
    for (int i=0; i < v.size(); i++)
    {
        ObjString *s = n[i];
        delete s;
    }
    v.clear();
    return true;
}
bool LinkDebugFile::WriteLineNumbers()
{
    std::vector<sqlite3_int64> v;
//    std::vector<sqlite3_int64> a;
    for (ObjFile::SectionIterator it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        ObjMemoryManager &m = (*it)->GetMemoryManager();
        ObjInt address = m.GetBase();
        for (ObjMemoryManager::MemoryIterator it1 = m.MemoryBegin(); it1 != m.MemoryEnd(); ++it1)
        {
            if ((*it1)->HasDebugTags())
            {
                ObjLineNo *ln = NULL;
                // full list of line numbers needed for symbol data...
                for (ObjMemory::DebugTagIterator it2 = (*it1)->DebugTagBegin(); it2 != (*it1)->DebugTagEnd(); ++it2)
                {
                    ObjLineNo *ln2 = (*it2)->GetLineNo();
                    if (ln2)
                    {
//                        int line = ln2->GetLineNumber();  
//                        int fileId = ln2->GetFile()->GetIndex();
//                        a.push_back(address);
//                        a.push_back(fileId);
//                        a.push_back(line);
                        ln = ln2;
                    }
                }
                // pick the last line number in the list for this address...
                if (ln)
                {
                    int line = ln->GetLineNumber();  
                    int fileId = ln->GetFile()->GetIndex();
                    v.push_back(address);
                    v.push_back(fileId);
                    v.push_back(line);
                }
            }
            address += (*it1)->GetSize();
        }
    }
    IntegerColumnsVirtualTable lines(v, 3);
    lines.Start(dbPointer);
    lines.InsertIntoFrom("linenumbers");
    lines.Stop();
//    IntegerColumnsVirtualTable linesfull(a, 3);
//    linesfull.Start(dbPointer);
//    linesfull.InsertIntoFrom("linenumbersfull");
//    linesfull.Stop();
    return true;
}
int LinkDebugFile::GetSQLNameId(ObjString name)
{
    std::map<ObjString, int>::iterator it = names.find(name);
    if (it != names.end())
    {
        return it->second;
    }
    nameList.push_back(new ObjString(name));
    names[name] = nameList.size();
    return nameList.size();
}
bool LinkDebugFile::WriteNamesTable()
{
    std::vector<sqlite3_int64> v;
    for (int i=0; i < nameList.size(); i++)
    {
        v.push_back(i+1);
    }
    LinkerColumnsWithNameVirtualTable fns(v, nameList, 2, true);
    fns.Start(dbPointer);
    fns.InsertIntoFrom("names");
    fns.Stop();
    for (int i=0; i < nameList.size(); i++)
    {
        ObjString *s = nameList[i];
        delete s;
    }
    nameList.clear();
    return true;
}
bool LinkDebugFile::WriteVariableTypes()
{
    ObjString empty = "";
    std::vector<sqlite3_int64> v;
    std::vector<ObjString *>n;
    for (ObjFile::TypeIterator it = file->TypeBegin(); it != file->TypeEnd(); ++it)
    {
        ObjType *type = *it;
        switch (type->GetType())
        {
            case ObjType::ePointer:
                v.push_back(type->GetIndex());
                v.push_back(ObjType::ePointer);
                v.push_back(type->GetSize());
                v.push_back(GetTypeIndex(type->GetBaseType()));
                v.push_back(0);
                v.push_back(0);
                v.push_back(0);
                v.push_back(0);
                v.push_back(0);
                v.push_back(0);
                n.push_back(&empty);
                break;
            case ObjType::eFunction:
            {
                ObjFunction *func = static_cast<ObjFunction *>(type);
                v.push_back(func->GetIndex());
                v.push_back(ObjType::eFunction);
                v.push_back(0);
                v.push_back(func->GetLinkage());
                v.push_back(0);
                v.push_back(GetTypeIndex(func->GetReturnType()));
                v.push_back(0);
                v.push_back(0);
                v.push_back(0);
                v.push_back(0);
                n.push_back(&empty);
            }
                break;
            case ObjType::eBitField:
                v.push_back(type->GetIndex());
                v.push_back(ObjType::eBitField);
                v.push_back(type->GetSize());
                v.push_back(GetTypeIndex(type->GetBaseType()));
                v.push_back(0);
                v.push_back(0);
                v.push_back(0);
                v.push_back(0);
                v.push_back(type->GetStartBit());
                v.push_back(type->GetBitCount());
                n.push_back(&empty);
                break;
            case ObjType::eStruct:
            case ObjType::eUnion:
            case ObjType::eEnum:
            {
                v.push_back(type->GetIndex());
                v.push_back(type->GetType());
                v.push_back(type->GetSize());
                v.push_back(type->GetBaseType() ? GetTypeIndex(type->GetBaseType()) : 0);
                v.push_back(0);
                v.push_back(0);
                v.push_back(0);
                v.push_back(0);
                v.push_back(0);
                v.push_back(0);
                n.push_back(&empty);
            }
                break;
            case ObjType::eArray:
            case ObjType::eVla:
                v.push_back(type->GetIndex());
                v.push_back(type->GetType());
                v.push_back(type->GetSize());
                v.push_back(GetTypeIndex(type->GetBaseType()));
                v.push_back(GetTypeIndex(type->GetIndexType()));
                v.push_back(0);
                v.push_back(type->GetBase());
                v.push_back(type->GetTop());
                v.push_back(0);
                v.push_back(0);
                n.push_back(&empty);
                break;
            case ObjType::eTypeDef:
                v.push_back(type->GetIndex());
                v.push_back(ObjType::eTypeDef);
                v.push_back(0);
                v.push_back(GetTypeIndex(type->GetBaseType()));
                v.push_back(0);
                v.push_back(0);
                v.push_back(0);
                v.push_back(0);
                v.push_back(0);
                v.push_back(0);
                n.push_back(new ObjString(type->GetName()));
                break;
        }
       
    }
    
    LinkerColumnsWithNameVirtualTable types(v, n, 11, true);
    types.Start(dbPointer);
    types.InsertIntoFrom("types");
    types.Stop();
    for (int i=0; i < n.size(); i++)
    {
        ObjString *s = n[i];
        if (s != &empty)
            delete s;
    }
    v.clear();
    for (ObjFile::TypeIterator it = file->TypeBegin(); it != file->TypeEnd(); ++it)
    {
        ObjType *type = *it;
        switch (type->GetType())
        {
            case ObjType::eFunction:
            {
                ObjFunction *func = static_cast<ObjFunction *>(type);
                int order = 0;
                for (ObjFunction::ParameterIterator it = func->ParameterBegin(); it != func->ParameterEnd(); ++it)
                {
                    v.push_back(func->GetIndex());
                    v.push_back(GetTypeIndex((*it)));
                    v.push_back(order++);
                }
            }
                break;
        }
       
    }
    IntegerColumnsVirtualTable args(v, 3);
    args.Start(dbPointer);
    args.InsertIntoFrom("args");
    args.Stop();
    v.clear();
    for (ObjFile::TypeIterator it = file->TypeBegin(); it != file->TypeEnd(); ++it)
    {
        ObjType *type = *it;
        switch (type->GetType())
        {
            case ObjType::eStruct:
            case ObjType::eUnion:
            case ObjType::eEnum:
            {
                int order = 0;
                for (ObjType::FieldIterator it = type->FieldBegin(); it != type->FieldEnd(); ++it)
                {
                    v.push_back(type->GetIndex());
                    v.push_back(GetTypeIndex((*it)->GetBase()));
                    v.push_back(GetSQLNameId((*it)->GetName()));
                    v.push_back((*it)->GetConstVal());
                    v.push_back(order++);
                }
            }
                break;
        }
       
    }
    IntegerColumnsVirtualTable fields(v, 5);
    fields.Start(dbPointer);
    fields.InsertIntoFrom("fields");
    fields.Stop();
    return true;
}
bool LinkDebugFile::WriteVariableNames()
{
    for (ObjFile::SymbolIterator it = file->PublicBegin(); it != file->PublicEnd(); ++it)
    {
        int index = (*it)->GetIndex();
        ObjString name = (*it)->GetName();
        int n = GetSQLNameId(name);
        if (n == -1)
            return false;
        publicMap[index] = n;
    }
    for (ObjFile::SymbolIterator it = file->LocalBegin(); it != file->LocalEnd(); ++it)
    {
        int index = (*it)->GetIndex();
        ObjString name = (*it)->GetName();
        int n = GetSQLNameId(name);
        if (n == -1)
            return false;
        localMap[index] = n;
    }
    for (ObjFile::SymbolIterator it = file->AutoBegin(); it != file->AutoEnd(); ++it)
    {
        int index = (*it)->GetIndex();
        ObjString name = (*it)->GetName();
        int n = GetSQLNameId(name);
        if (n == -1)
            return false;
        autoMap[index] = n;
    }
    for (ObjFile::TypeIterator it = file->TypeBegin(); it != file->TypeEnd(); ++it)
    {
        ObjString name = (*it)->GetName();
        if (name.size())
        {
            int index = (*it)->GetIndex();
            int n = GetSQLNameId(name);
            if (n == -1)
                return false;
            typeMap[index] = n;
        }
    }
    return true;
}
ObjInt LinkDebugFile::GetSectionBase(ObjExpression *e)
{
    if (e->GetOperator() == ObjExpression::eAdd)
    {
        int rv = GetSectionBase(e->GetLeft());
        if (!rv)
            rv = GetSectionBase(e->GetRight());
        return rv;
    }
    else if (e->GetOperator() == ObjExpression::eSection)
    {
        ObjSection *s = file->GetSection(e->GetSection()->GetIndex());
        ObjMemoryManager &m = s->GetMemoryManager();
        // wrecks the link but the link is already done!!!!
        e->GetSection()->SetOffset(new ObjExpression(0));
        return m.GetBase();
    }
    return 0;
}
bool LinkDebugFile::WriteGlobalsTable()
{
    std::vector<sqlite3_int64> v;
    for (ObjFile::SymbolIterator it = file->PublicBegin(); it != file->PublicEnd(); ++it)
    {
        int index = (*it)->GetIndex();
        //ObjString name = (*it)->GetName();
        index = publicMap[index];
        int address = GetSectionBase((*it)->GetOffset());
        address += (*it)->GetOffset()->EvalNoModify(0);
        int type = -1;
        if ((*it)->GetBaseType())
            type = GetTypeIndex((*it)->GetBaseType());
        int fileId = (*it)->GetSourceFile() ? (*it)->GetSourceFile()->GetIndex() : 0;
        v.push_back(index);
        v.push_back(type);
        v.push_back(address);
        v.push_back(fileId);
    }
    IntegerColumnsVirtualTable globals(v, 4);
    globals.Start(dbPointer);
    globals.InsertIntoFrom("globals");
    globals.Stop();
    v.clear();
    for (ObjFile::SymbolIterator it = file->LocalBegin(); it != file->LocalEnd(); ++it)
    {
        int index = (*it)->GetIndex();
        //ObjString name = (*it)->GetName();
        index = localMap[index];
        int address = GetSectionBase((*it)->GetOffset());
        address += (*it)->GetOffset()->EvalNoModify(0);
        int type = -1;
        if ((*it)->GetBaseType())
            type = GetTypeIndex((*it)->GetBaseType());
        int fileId = (*it)->GetSourceFile() ? (*it)->GetSourceFile()->GetIndex() : 0;
        v.push_back(index);
        v.push_back(type);
        v.push_back(address);
        v.push_back(fileId);
    }
    IntegerColumnsVirtualTable locals(v, 4);
    locals.Start(dbPointer);
    locals.InsertIntoFrom("locals");
    locals.Stop();

    return true;
}
bool LinkDebugFile::WriteAutosTable()
{
    std::vector<sqlite3_int64> v;
    class context
    {
    public:
        context() : startLine(NULL), currentLine(NULL) { }
        ObjLineNo *startLine;
        ObjLineNo *currentLine;
        std::deque<ObjSymbol *>vars;
    } ;
    std::deque<context *> contexts;
    context *currentContext = NULL;
    int funcId = 0;
    for (ObjFile::SectionIterator it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        ObjMemoryManager &m = (*it)->GetMemoryManager();
        ObjInt address = m.GetBase();
        ObjLineNo *currentLine = NULL;
        for (ObjMemoryManager::MemoryIterator it1 = m.MemoryBegin(); it1 != m.MemoryEnd(); ++it1)
        {
            // find the line number first.  This is because it is difficult to get the line number
            // out of the compiler before the variable name is listed.
            if ((*it1)->HasDebugTags())
            {
                for (ObjMemory::DebugTagIterator it2 = (*it1)->DebugTagBegin(); it2 != (*it1)->DebugTagEnd(); ++it2)
                {
                    switch((*it2)->GetType())
                    {
                        case ObjDebugTag::eLineNo:
                            currentLine = (*it2)->GetLineNo();
                            break;
                    }
                }
                for (ObjMemory::DebugTagIterator it2 = (*it1)->DebugTagBegin(); it2 != (*it1)->DebugTagEnd(); ++it2)
                {
                    switch((*it2)->GetType())
                    {
                        case ObjDebugTag::eVar:
                            currentContext->vars.push_back((*it2)->GetSymbol());
                            break;
                        case ObjDebugTag::eFunctionStart:
                        {
                            ObjSymbol *func = (*it2)->GetSymbol();
                            if (func->GetType() == ObjSymbol::ePublic)
                            {
                                funcId = publicMap[func->GetIndex()];
                            }
                            else
                            {
                                funcId = localMap[func->GetIndex()];
                            }
                        }
                            // fallthrough
                        case ObjDebugTag::eBlockStart:
                            contexts.push_back(currentContext);
                            currentContext = new context;
                            currentContext->startLine = currentContext->currentLine = currentLine;
                            break;
                        case ObjDebugTag::eFunctionEnd:
                        {
                            ObjSymbol *func = (*it2)->GetSymbol();
                        }
                        case ObjDebugTag::eBlockEnd:
                        {
                            int start = currentContext->startLine->GetLineNumber();  
                            int end = currentContext->currentLine->GetLineNumber();
                            int fileId = currentContext->startLine->GetFile()->GetIndex();
                            for (std::deque<ObjSymbol *>::iterator it = currentContext->vars.begin(); it != currentContext->vars.end(); ++it)
                            {
                                ObjSymbol *s = (*it);
                                v.push_back(autoMap[s->GetIndex()]);
                                v.push_back(GetTypeIndex(s->GetBaseType()));
                                v.push_back(funcId);
                                v.push_back(fileId);
                                v.push_back(s->GetOffset()->EvalNoModify(0));
                                v.push_back(start);
                                v.push_back(end);
                            }
                            delete currentContext;
                            currentContext = contexts.back();
                            contexts.pop_back();
                            break;
                        }
                    }
                    if (currentContext)
                        currentContext->currentLine = currentLine;
                }
            }
            address += (*it1)->GetSize();
        }
    }
    IntegerColumnsVirtualTable autos(v, 7);
    autos.Start(dbPointer);
    autos.InsertIntoFrom("autos");
    autos.Stop();
    contexts.clear();

    return true;
}
bool LinkDebugFile::WriteTypeNamesTable()
{
    std::vector<sqlite3_int64> v;
    for (ObjFile::TypeIterator it = file->TypeBegin(); it != file->TypeEnd(); ++it)
    {
        ObjString name = (*it)->GetName();
        if (name.size())
        {
            int index = (*it)->GetIndex();
            v.push_back(typeMap[index]);
            v.push_back(index);
        }
    }
    IntegerColumnsVirtualTable tns(v, 2, true);
    tns.Start(dbPointer);
    tns.InsertIntoFrom("typenames");
    tns.Stop();
    return true;
}
bool LinkDebugFile::CreateOutput()
{
    bool ok = DBOpen((char *)outputFile.c_str());
    if (ok)
    {
        ok = false;
        Begin();
        if (WriteFileNames())
            if (WriteLineNumbers())
                if (WriteVariableTypes())
                    if (WriteVariableNames())
                       if (WriteGlobalsTable())
                            if (WriteAutosTable())
                                if (WriteTypeNamesTable())
                                    if (WriteNamesTable())
                                        if (CreateIndexes())
                                            ok = true;
        End();
    }
    if (dbPointer)
        sqlite3_close(dbPointer);
    return ok;
}
