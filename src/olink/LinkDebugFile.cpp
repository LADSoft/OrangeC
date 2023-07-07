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
#include <cstdio>
#include <deque>
#include <set>
#include <cctype>
#include <iostream>
#include <algorithm>
#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#endif
#define STRINGVERSION "120"
#define DBVersion atoi(STRINGVERSION)

static int version_ok;

// well I didn't originally use virtual tables.  It turns out that virtual tables
// speed things up for individual tables, but when you are writing as database the
// disk writes are the limiting factor.  Nonetheless I remain with the virtual table
// approach because the codelooks much prettier...
const char* LinkDebugFile::pragmas = {"PRAGMA journal_mode=MEMORY; PRAGMA temp_store=MEMORY;"};
const char* LinkDebugFile::tables = {
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
    " id INTEGER PRIMARY KEY"  // groups types
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
    " typeId INTEGER"  // groups types
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
    "CREATE TABLE Virtuals ("
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
    "CREATE TABLE CPPNameMapping ("
    "  simpleId INTEGER"
    " ,complexId INTEGER"
    " );"
    "INSERT INTO dbPropertyBag (property, value)"
    " VALUES (\"dbVersion\", " STRINGVERSION
    ");"
    "COMMIT; "};
const char* LinkDebugFile::indexes = {
    "BEGIN; "
    "CREATE INDEX LNIndex ON LineNumbers(fileid,line);"
    "CREATE INDEX TNIndex1 ON TypeNames(symbolId);"
    "CREATE INDEX TNIndex2 ON TypeNames(typeId);"
    "CREATE INDEX FDIndex ON Fields(typeId);"
    "CREATE INDEX ARIndex ON Args(typeId);"
    "CREATE INDEX GBLIndex1 ON Globals(symbolId, fileId);"
    "CREATE INDEX GBLIndex2 ON Globals(varAddress, fileId);"
    "CREATE INDEX LCLIndex ON Locals(symbolId, fileId);"
    "CREATE INDEX AUTIndex ON Autos(symbolId, fileId, funcId);"
    "COMMIT; "};
LinkDebugFile::~LinkDebugFile() {}
bool LinkDebugFile::Begin(void) { return SQLiteExec("BEGIN"); }
bool LinkDebugFile::End(void) { return SQLiteExec("END"); }
bool LinkDebugFile::SQLiteExec(const char* str)
{
    char* zErrMsg = 0;
    int rc = sqlite3_exec(dbPointer, str, 0, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return false;
    }
    return true;
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
        return false;
    }
    return rv;
}
bool LinkDebugFile::CreateIndexes(void) { return SQLiteExec(indexes); }
bool LinkDebugFile::DBOpen(char* name)
{
    bool rv = false;
    dbPointer = nullptr;
    _unlink(name);
    if (sqlite3_open_v2(name, &dbPointer, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr) == SQLITE_OK)
    {
        rv = CreateTables();
    }
    else
    {
        // we don't want to reuse a database for a variety of reasons...
        return false;
    }
    if (rv)
        sqlite3_busy_timeout(dbPointer, 100);
    return rv;
}
bool LinkDebugFile::WriteFileNames()
{
    std::vector<sqlite3_int64> v;
    std::vector<ObjString> n;
    for (auto it = file->SourceFileBegin(); it != file->SourceFileEnd(); ++it)
    {
        ObjString name = (*it)->GetName();
        int index = (*it)->GetIndex();

        std::transform(name.begin(), name.end(), name.begin(), tolower);
        v.push_back(index);
        n.push_back(name);
    }
    LinkerColumnsWithNameVirtualTable fns(v, n, 2, true);
    fns.Start(dbPointer);
    fns.InsertIntoFrom("filenames");
    fns.Stop();
    v.clear();
    return true;
}
bool LinkDebugFile::WriteLineNumbers()
{
    std::vector<sqlite3_int64> v;
    //    std::vector<sqlite3_int64> a;
    for (auto it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        ObjMemoryManager& m = (*it)->GetMemoryManager();
        ObjInt address = m.GetBase();
        for (auto memory : m)
        {
            if (memory->HasDebugTags())
            {
                ObjLineNo* ln = nullptr;
                // full list of line numbers needed for symbol data...
                for (auto tag : *memory)
                {
                    ObjLineNo* ln2 = tag->GetLineNo();
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
            address += memory->GetSize();
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
void LinkDebugFile::PushCPPName(const ObjString& name, int n)
{
    if (name.find("@") != std::string::npos)
    {
        int first = 0;
        int last = name.size();
        int nested = 0;
        for (int i = 0; i < name.size(); i++)
        {
            if (name[i] == '@')
            {
                first = i;
                if (name[i + 1] == '$')
                    i++;  // past any '$'
            }
            else if (name[i] == '#')
            {
                nested++;
            }
            else if (name[i] == '~')
            {
                nested--;
            }
            else if (!nested && name[i] == '$')
            {
                last = i;
                break;
            }
        }
        if (last != name.size())
        {
            std::string simpleName = name.substr(first, last - first);
            int s = GetSQLNameId(simpleName);
            CPPMapping map;
            map.simpleId = s;
            map.complexId = n;
            CPPMappingList.push_back(map);

            if (first != 0)
            {
                simpleName = name.substr(0, last);
                s = GetSQLNameId(simpleName);
                map.simpleId = s;
                map.complexId = n;
                CPPMappingList.push_back(map);
            }
        }
    }
}
int LinkDebugFile::GetSQLNameId(const ObjString& name)
{
    auto it = names.find(name);
    if (it != names.end())
    {
        return it->second;
    }
    nameList.push_back(name);
    int n = names[name] = nameList.size();
    PushCPPName(name, n);
    return n;
}
bool LinkDebugFile::WriteNamesTable()
{
    std::vector<sqlite3_int64> v;
    for (int i = 0; i < nameList.size(); i++)
    {
        v.push_back(i + 1);
    }
    LinkerColumnsWithNameVirtualTable fns(v, nameList, 2, true);
    fns.Start(dbPointer);
    fns.InsertIntoFrom("names");
    fns.Stop();
    nameList.clear();
    v.clear();
    for (auto map : CPPMappingList)
    {
        v.push_back(map.simpleId);
        v.push_back(map.complexId);
    }
    IntegerColumnsVirtualTable args(v, 2);
    args.Start(dbPointer);
    args.InsertIntoFrom("cppnamemapping");
    args.Stop();
    CPPMappingList.clear();
    return true;
}
bool LinkDebugFile::WriteVariableTypes()
{
    ObjString empty = "";
    std::vector<sqlite3_int64> v;
    std::vector<ObjString> n;
    for (auto it = file->TypeBegin(); it != file->TypeEnd(); ++it)
    {
        ObjType* type = *it;
        switch (type->GetType())
        {
            case ObjType::ePointer:
            case ObjType::eLRef:
            case ObjType::eRRef:
                v.push_back(type->GetIndex());
                v.push_back(type->GetType());
                v.push_back(type->GetSize());
                v.push_back(GetTypeIndex(type->GetBaseType()));
                v.push_back(0);
                v.push_back(0);
                v.push_back(0);
                v.push_back(0);
                v.push_back(0);
                v.push_back(0);
                n.push_back("");
                break;
            case ObjType::eFunction: {
                ObjFunction* func = static_cast<ObjFunction*>(type);
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
                n.push_back("");
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
                n.push_back("");
                break;
            case ObjType::eStruct:
            case ObjType::eUnion:
            case ObjType::eEnum: {
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
                n.push_back("");
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
                n.push_back("");
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
                n.push_back(type->GetName());
                break;
            default:
                break;
        }
    }
    LinkerColumnsWithNameVirtualTable types(v, n, 11, true);
    types.Start(dbPointer);
    types.InsertIntoFrom("types");
    types.Stop();
    v.clear();
    for (auto it = file->TypeBegin(); it != file->TypeEnd(); ++it)
    {
        ObjType* type = *it;
        switch (type->GetType())
        {
            case ObjType::eFunction: {
                ObjFunction* func = static_cast<ObjFunction*>(type);
                int order = 0;
                for (auto param : *func)
                {
                    v.push_back(func->GetIndex());
                    v.push_back(GetTypeIndex(param));
                    v.push_back(order++);
                }
            }
            break;
            default:
                break;
        }
    }
    IntegerColumnsVirtualTable args(v, 3);
    args.Start(dbPointer);
    args.InsertIntoFrom("args");
    args.Stop();
    v.clear();
    for (auto it = file->TypeBegin(); it != file->TypeEnd(); ++it)
    {
        ObjType* type = *it;
        switch (type->GetType())
        {
            case ObjType::eStruct:
            case ObjType::eUnion:
            case ObjType::eEnum: {
                int order = 0;
                for (auto field : *type)
                {
                    v.push_back(type->GetIndex());
                    v.push_back(GetTypeIndex(field->GetBase()));
                    v.push_back(GetSQLNameId(field->GetName()));
                    v.push_back(field->GetConstVal());
                    v.push_back(order++);
                }
            }
            break;
            default:
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
    for (auto it = file->PublicBegin(); it != file->PublicEnd(); ++it)
    {
        int index = (*it)->GetIndex();
        ObjString name = (*it)->GetName();
        int n = GetSQLNameId(name);
        if (n == -1)
            return false;
        publicMap[index] = n;
    }
    for (auto it = file->LocalBegin(); it != file->LocalEnd(); ++it)
    {
        int index = (*it)->GetIndex();
        ObjString name = (*it)->GetName();
        int n = GetSQLNameId(name);
        if (n == -1)
            return false;
        localMap[index] = n;
    }
    for (auto it = file->AutoBegin(); it != file->AutoEnd(); ++it)
    {
        int index = (*it)->GetIndex();
        ObjString name = (*it)->GetName();
        int n = GetSQLNameId(name);
        if (n == -1)
            return false;
        autoMap[index] = n;
    }
    for (auto it = file->TypeBegin(); it != file->TypeEnd(); ++it)
    {
        ObjString name = (*it)->GetName();
        if (!name.empty())
        {
            int index = (*it)->GetIndex();
            int n = GetSQLNameId(name);
            if (n == -1)
                return false;
            typeMap[index] = n;
        }
    }
    for (auto sect : Sections)
    {
        ObjString name = sect->GetName();
        int npos = name.find("@");
        if (npos != std::string::npos && npos != name.size() - 1)
            name = name.substr(npos);
        int n = GetSQLNameId(name);
        if (n == -1)
            return false;
        sectionMap[sect->GetName()] = n;
    }
    return true;
}
ObjInt LinkDebugFile::GetSectionBase(ObjExpression* e)
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
        ObjSection* s = file->GetSection(e->GetSection()->GetIndex());
        ObjExpression* oldOffs = e->GetSection()->GetOffset();
        ObjMemoryManager& m = s->GetMemoryManager();
        // wrecks the link but the link is already done!!!!
        e->GetSection()->SetOffset(new ObjExpression(0));
        int rv = m.GetBase();
        e->GetSection()->SetOffset(oldOffs);
        return rv;
    }
    return 0;
}
bool LinkDebugFile::WriteGlobalsTable()
{
    std::vector<sqlite3_int64> v;
    for (auto it = file->PublicBegin(); it != file->PublicEnd(); ++it)
    {
        int index = (*it)->GetIndex();
        // ObjString name = (*it)->GetName();
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
    for (auto it = file->LocalBegin(); it != file->LocalEnd(); ++it)
    {
        int index = (*it)->GetIndex();
        // ObjString name = (*it)->GetName();
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

    v.clear();
    for (auto sect : Sections)
    {
        int n = sectionMap[sect->GetName()];
        int address = ParentSections[sect]->GetBase() + sect->GetBase();
        int type = -1;
        if (sect->GetVirtualType())
            type = GetTypeIndex(sect->GetVirtualType());
        int fileId = /*(*it)->GetSourceFile() ? sect->GetSourceFile()->GetIndex() :*/ 0;
        v.push_back(n);
        v.push_back(type);
        v.push_back(address);
        v.push_back(fileId);
    }
    IntegerColumnsVirtualTable virtuals(v, 4);
    virtuals.Start(dbPointer);
    virtuals.InsertIntoFrom("virtuals");
    virtuals.Stop();

    return true;
}
class context
{
  public:
    context() : startLine(nullptr), currentLine(nullptr) {}
    ObjLineNo* startLine;
    ObjLineNo* currentLine;
    std::map<ObjSymbol*, ObjLineNo*> vars;
};
bool LinkDebugFile::WriteAutosTable()
{
    std::vector<sqlite3_int64> v;
    std::deque<std::unique_ptr<context>> contexts;
    std::unique_ptr<context> currentContext;
    int funcId = 0;
    for (auto it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        ObjMemoryManager& m = (*it)->GetMemoryManager();
        ObjInt address = m.GetBase();
        ObjLineNo* currentLine = nullptr;
        for (auto mem : m)
        {
            // find the line number first.  This is because it is difficult to get the line number
            // out of the compiler before the variable name is listed.
            if (mem->HasDebugTags())
            {
                for (auto tag : *mem)
                {
                    switch (tag->GetType())
                    {
                        case ObjDebugTag::eLineNo:
                            if (tag->GetLineNo() > currentLine)  // e.g. for statements can have their first statement last
                                currentLine = tag->GetLineNo();
                            break;
                        default:
                            break;
                    }
                }
                for (auto tag : *mem)
                {
                    switch (tag->GetType())
                    {
                        case ObjDebugTag::eLineNo:
                            currentLine = tag->GetLineNo();
                            break;
                        case ObjDebugTag::eVar:

                            currentContext->vars[tag->GetSymbol()] = currentLine;
                            break;
                        case ObjDebugTag::eVirtualFunctionStart: {
                            ObjSection* func = tag->GetSection();
                            funcId = sectionMap[func->GetName()];
                        }
                            // fall through
                        case ObjDebugTag::eFunctionStart:
                            if (tag->GetType() == ObjDebugTag::eFunctionStart)
                            {
                                ObjSymbol* func = tag->GetSymbol();
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

                            contexts.push_back(std::move(currentContext));
                            currentContext = std::make_unique<context>();
                            currentContext->startLine = currentContext->currentLine = currentLine;
                            break;
                        case ObjDebugTag::eVirtualFunctionEnd:
                        case ObjDebugTag::eBlockEnd:
                        case ObjDebugTag::eFunctionEnd: {

                            int end = currentLine->GetLineNumber();
                            // this next line is a safety, used to use start line
                            int fileId = currentLine->GetFile()->GetIndex();
                            for (auto obj : currentContext->vars)
                            {
                                ObjSymbol* s = obj.first;
                                int startLine = obj.second->GetLineNumber();
                                v.push_back(autoMap[s->GetIndex()]);
                                v.push_back(GetTypeIndex(s->GetBaseType()));
                                v.push_back(funcId);
                                v.push_back(fileId);
                                v.push_back(s->GetOffset()->EvalNoModify(0));
                                v.push_back(startLine);
                                v.push_back(end);
                            }
                            currentContext.reset(contexts.back().release());
                            contexts.pop_back();
                            break;
                        }
                        default:
                            break;
                    }
                    if (currentContext)
                        currentContext->currentLine = currentLine;
                }
            }
            address += mem->GetSize();
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
    for (auto it = file->TypeBegin(); it != file->TypeEnd(); ++it)
    {
        ObjString name = (*it)->GetName();
        if (!name.empty())
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
    bool ok = DBOpen((char*)outputFile.c_str());
    if (ok)
    {
        ok = false;
        //        Begin();
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
        //        End();
    }
    if (dbPointer)
        sqlite3_close(dbPointer);
    return ok;
}
