/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
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

#include "BRCDictionary.h"
#include "BRCWriter.h"
#include "ObjBrowseInfo.h"
#include <stdio.h>
#include <algorithm>
#include <string.h>
#include <stdlib.h>
#include <io.h>

#define STRINGVERSION "120"

#define DBVersion atoi(STRINGVERSION)

#define JT_NONE 0
#define JT_FUNC 1
#define JT_VAR 2
#define JT_DEFINE 4
#define JT_TYPEDATA 8
#define JT_LOCALDATA 0x10
#define JT_STATIC 0x20
#define JT_GLOBAL 0x40
#define JT_MAPPING 0x1000

static int version_ok;

char *BRCWriter::tables= 
{
    "PRAGMA journal_mode=MEMORY; PRAGMA temp_store = MEMORY; "
    "BEGIN; "
    "CREATE TABLE brPropertyBag ("
    " property VARCHAR(100)"
    " ,value VARCHAR(200)"
    " );"
    "CREATE TABLE Names ("
    " id INTEGER PRIMARY KEY AUTOINCREMENT"
    " ,name VARCHAR(200)"
    " ,type INTEGER"
    " );"
    "CREATE TABLE FileNames ("
    " id INTEGER PRIMARY KEY AUTOINCREMENT"
    " ,name VARCHAR(260)"
    " ,fileDate DATE"
    " );"
    "CREATE TABLE LineNumbers ("
    " type INTEGER"
    " ,qual INTEGER"
    " ,symbolId INTEGER"
    " ,fileId INTEGER"
    " ,startLine INTEGER"
    " ,charPos INTEGER"
    " ,hint VARCHAR(256)"
    " ,FOREIGN KEY (symbolId) REFERENCES Names(id)"
    " ,FOREIGN KEY (fileId) REFERENCES FileNames(id)"
    " );"
    "CREATE TABLE Usages ("
    " type INTEGER"
    " ,qual INTEGER"
    " ,symbolId INTEGER"
    " ,fileId INTEGER"
    " ,startLine INTEGER"
    " ,charPos INTEGER"
    " ,hint VARCHAR(256)"
    " ,FOREIGN KEY (symbolId) REFERENCES Names(id)"
    " ,FOREIGN KEY (fileId) REFERENCES FileNames(id)"
    " );"
    "CREATE TABLE JumpTable ("
    " symbolId INTEGER"
    " ,fileId INTEGER"
    " ,startLine INTEGER"
    " ,endLine INTEGER"
    " ,FOREIGN KEY (symbolId) REFERENCES Names(id)"
    " );"
    "CREATE TABLE CPPNameMapping ("
    "  simpleId INTEGER"
    " ,complexId INTEGER"
    " );"
    "INSERT INTO brPropertyBag (property, value)"
    " VALUES (\"brVersion\", " STRINGVERSION ");"
    "COMMIT; "
} ;
char *BRCWriter::deletion =
{
    "BEGIN;"
    "DELETE FROM Names;"
    "DELETE FROM FileNames;"
    "DELETE FROM LineNumbers;"
    "DELETE FROM Usages;"
    "DELETE FROM JumpTable;"
    "DELETE FROM CPPNameMapping;"
    "COMMIT;"
};
BRCWriter::~BRCWriter()
{
}
bool BRCWriter::Begin(void)
{
    bool rv = true;
    if (!SQLiteExec("BEGIN"))
    {
        rv = false;
    }
    return rv;
}
bool BRCWriter::End(void )
{
    bool rv = true;
    if (!SQLiteExec("COMMIT"))
    {
        rv = false;
    }
    return rv;
}
int BRCWriter::NullCallback(void *NotUsed, int argc, char **argv, char **azColName)
{
    return 0;
}
int BRCWriter::SQLiteExec( char *str)
{
    char *zErrMsg  = 0;
    int rv = false;
    int rc = sqlite3_exec(dbPointer, str, NullCallback, 0, &zErrMsg);
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
int BRCWriter::VersionCallback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    if (argc == 1)
    {
        if (atoi(argv[0]) >= DBVersion)
            version_ok = true;
    }
    return 0;
}
int BRCWriter::CheckDb(void)
{
    char *zErrMsg  = 0;
    int rv = false;
    int rc = sqlite3_exec(dbPointer, "SELECT value FROM brPropertyBag WHERE property = \"brVersion\"", 
                          VersionCallback, 0, &zErrMsg);
    if( rc!=SQLITE_OK )
    {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
        rv = version_ok;
    }
    return rv;
}
int BRCWriter::CreateTables(void)
{
    int rv = true;
    if (!SQLiteExec(tables))
    {
        rv = false;
    }
    return rv;
}
int BRCWriter::DBOpen(char *name)
{
    int rv = false;
    _unlink(name);
    if (sqlite3_open_v2(name, &dbPointer,SQLITE_OPEN_READWRITE, nullptr) == SQLITE_OK)
    {
#ifdef TEST
        printf("checkdb...");
#endif
        rv = CheckDb();
        if (!rv || !SQLiteExec(deletion))
        {
            sqlite3_close(dbPointer);
            _unlink(name);
            goto doCreate;
        }
    }
    else 
    {
        if (dbPointer)
            sqlite3_close(dbPointer);
doCreate:
        dbPointer = nullptr;
        if (sqlite3_open_v2(name, &dbPointer, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr) == SQLITE_OK)
        {
#ifdef TEST
            printf("createdb...");
#endif
            rv = CreateTables();
        }
        else
        {
#ifdef TEST
            printf("open...");
#endif
        }
    }
    if (rv)
        sqlite3_busy_timeout(dbPointer, 800);
    return rv;
}
bool BRCWriter::Insert(sqlite3_int64 simpleId, sqlite3_int64 complexId)
{
    static char *query = "INSERT INTO CPPNameMapping (simpleId, complexId) VALUES (?, ?)";
    int rc = SQLITE_OK;
    static sqlite3_stmt *handle;
    if (!handle)
    {
        rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, nullptr);
    }
    if (rc == SQLITE_OK)
    {
        int done = false;
        sqlite3_reset(handle);
        sqlite3_bind_int(handle, 1, simpleId);
        sqlite3_bind_int(handle, 2, complexId);
        while (!done)
        {
            switch(rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = true;
                    break;
                case SQLITE_DONE:
                    rc = SQLITE_OK;
                    done = true;
                    break;
                default:
                    done = true;
                    break;
            }
        }                        
    }
    return rc == SQLITE_OK;
}
bool BRCWriter::Insert(std::string fileName, int index)
{
    static char *query = "INSERT INTO FileNames (name) VALUES (?)";
    int rc = SQLITE_OK;
    static sqlite3_stmt *handle;
    if (!handle)
    {
        rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, nullptr);
    }
    if (rc == SQLITE_OK)
    {
        int done = false;
        sqlite3_reset(handle);
        sqlite3_bind_text(handle, 1, fileName.c_str(), fileName.size(), SQLITE_STATIC);
        while (!done)
        {
            switch(rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = true;
                    break;
                case SQLITE_DONE:
                    rc = SQLITE_OK;
                    done = true;
                    break;
                default:
                    done = true;
                    break;
            }
        }                        
    }
    if (rc == SQLITE_OK)
    {
        fileMap[index] = sqlite3_last_insert_rowid(dbPointer);
    }
    return rc == SQLITE_OK;
    
}
bool BRCWriter::Insert(std::string symName, int type, sqlite3_int64 *id)
{
    static char *query = "INSERT INTO Names (name, type) VALUES (?,?)";
    int rc = SQLITE_OK;
    static sqlite3_stmt *handle;
    if (!handle)
    {
        rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, nullptr);
    }
    if (rc == SQLITE_OK)
    {
        int done = false;
        sqlite3_reset(handle);
        sqlite3_bind_text(handle, 1, symName.c_str(), symName.size(), SQLITE_STATIC);
        sqlite3_bind_int(handle, 2, type);
        while (!done)
        {
            switch(rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = true;
                    break;
                case SQLITE_DONE:
                    rc = SQLITE_OK;
                    done = true;
                    break;
                default:
                    done = true;
                    break;
            }
        }                        
    }
    if (rc == SQLITE_OK)
    {
        *id = sqlite3_last_insert_rowid(dbPointer);
    }
    return rc == SQLITE_OK;
}
bool BRCWriter::Insert(sqlite3_int64 fileId, int start, int end, sqlite3_int64 nameIndex)
{
    static char *query = "INSERT INTO JumpTable (symbolId, fileId, startLine, endLine)"
                         " VALUES (?,?,?,?)";
    int rc = SQLITE_OK;
    static sqlite3_stmt *handle;
    if (!handle)
    {
        rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, nullptr);
    }
    if (rc == SQLITE_OK)
    {
        int done = false;
        sqlite3_reset(handle);
        sqlite3_bind_int64(handle, 1, nameIndex);
        sqlite3_bind_int64(handle, 2, fileId);
        sqlite3_bind_int(handle, 3, start);
        sqlite3_bind_int(handle, 4, end);
        while (!done)
        {
            switch(rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = true;
                    break;
                case SQLITE_DONE:
                    rc = SQLITE_OK;
                    done = true;
                    break;
                default:
                    done = true;
                    break;
            }
        }                        
    }
    return rc == SQLITE_OK;
}
bool BRCWriter::Insert(sqlite3_int64 symIndex, BrowseData *b, bool usages)
{
    static char *query = "INSERT INTO LineNumbers (type, qual, symbolId, fileId, startLine, charPos, hint)"
                         " VALUES (?,?,?,?,?,?,?)";
    static char *query1 = "INSERT INTO Usages (type, qual, symbolId, fileId, startLine, charPos, hint)"
                         " VALUES (?,?,?,?,?,?,?)";
    int rc = SQLITE_OK;
    static sqlite3_stmt *handle, *handle1;
    if (!handle)
    {
        rc = sqlite3_prepare_v2(dbPointer, query1, strlen(query1)+1, &handle1, nullptr);
        rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, nullptr);
    }
    if (rc == SQLITE_OK)
    {
        int done = false;
        sqlite3_stmt *iHandle = usages ? handle1 : handle;
        sqlite3_reset(iHandle);
        sqlite3_bind_int(iHandle, 1, b->type | (b->blockLevel == 0 ? 0x4000 : 0));
        sqlite3_bind_int(iHandle, 2, b->qual);
        sqlite3_bind_int64(iHandle, 3, symIndex);
        sqlite3_bind_int64(iHandle, 4, fileMap[b->fileIndex]);
        sqlite3_bind_int(iHandle, 5, b->startLine);
        sqlite3_bind_int(iHandle, 6, b->charPos);
        sqlite3_bind_text(iHandle, 7, b->hint.c_str(), b->hint.size(), SQLITE_STATIC);
        while (!done)
        {
            switch(rc = sqlite3_step(iHandle))
            {
                case SQLITE_BUSY:
                    done = true;
                    break;
                case SQLITE_DONE:
                    rc = SQLITE_OK;
                    done = true;
                    break;
                default:
                    done = true;
                    break;
            }
        }                        
    }
    return rc == SQLITE_OK;
}
char easytolower(char in){
  if(in<='Z' && in>='A')
    return in-('Z'-'z');
  return in;
} 
bool BRCWriter::WriteFileList()
{
    Begin();
    std::map<std::string, int> &unsorted = loader.GetFileNames();
    for (auto item : unsorted)
    {
        std::string myString = item.first;
        std::transform(myString.begin(), myString.end(), myString.begin(), easytolower); 
        if (!Insert(myString.c_str(), item.second))
            return false;
    }
    End();
    return true;
}

void BRCWriter:: InsertMappingSym(std::string name, SymData *orig, std::map<std::string, SymData *> &syms,
                 std::map<std::string, SymData *> &newSyms)
{
    SymData *sym = nullptr;
    auto it = syms.find(name);
    if (it != syms.end())
    {
         sym = it->second;
    }
    else {
        it = newSyms.find(name);
        if (it != newSyms.end())
            sym = it->second;
    }
    if (!sym)
    {
        sym = new SymData(name);
        newSyms[name] = sym;
    }
    sym->mapping.push_back(orig);
}
void BRCWriter::PushCPPNames(std::string name, SymData * orig, std::map<std::string, SymData *> &syms, 
                 std::map<std::string, SymData *> &newSyms)
{
    if (name.find("@") != std::string::npos)
    {
        int first = 0;
        int last = name.size();
        int nested = 0;
        for (int i =0; i < name.size(); i++)
        {
            if (name[i] == '@')
            {
                first = i;
                if (name[i+1] == '$')
                    i++; // past any '$'
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
            std::string simpleName = name.substr(first, last-first);
            InsertMappingSym( simpleName, orig, syms, newSyms);

            if (first != 0)
            {
                simpleName = name.substr(0, last);
                InsertMappingSym( simpleName, orig, syms, newSyms);
            }
        }
    }
}
bool BRCWriter::WriteDictionary(Symbols &syms)
{
    std::map<std::string, SymData *> newSyms;
    for (auto sym : syms)
    {
        PushCPPNames(sym.first, sym.second, syms, newSyms);        
    }
    for (auto sym : newSyms)
    {
        syms[sym.first] = sym.second;
    }
    newSyms.clear();
    Begin();
    for (auto sym1 : syms)
    {
        SymData *sym = sym1.second;
        int type = sym->mapping.size() ? JT_MAPPING : 0;
        for (auto l : sym->data)
        {
            if (!l->blockLevel && l->qual != ObjBrowseInfo::eExternal)
            {
                type |= JT_GLOBAL;
            }
            else if (l->blockLevel)
            {
                type |= JT_LOCALDATA;
            }
            if (l->qual == ObjBrowseInfo::eStatic)
                type |= JT_STATIC;
            if (l->qual == ObjBrowseInfo::eTypeval)
                type |= JT_TYPEDATA;
            switch (l->type)
            {
                case ObjBrowseInfo::eFuncStart:
                case ObjBrowseInfo::eTypePrototype:
                    type |= JT_FUNC;
                    break;
                case ObjBrowseInfo::eVariable:
                    if (!(type & JT_TYPEDATA))
                        type |= JT_VAR;
                    break;
                case ObjBrowseInfo::eDefine:
                    type |= JT_DEFINE;
                    break;
                default:
                    type |= JT_NONE;
                    break;
            }
        }	
        if (!Insert(sym1.first.c_str(), type, &sym->index))
            return false;
    }
    End();
    return true;
}
bool BRCWriter::WriteMapping(Symbols &syms)
{
    Begin();
    for (auto sym : syms)
    {
        SymData *s = sym.second;
        for (auto map : s->mapping)
            Insert(s->index, map->index);
    }
    End();
    return true;
}
bool BRCWriter::WriteLineData(Symbols &syms)
{
    Begin();
    for (auto sym : syms)
    {
        SymData *s = sym.second;
        for (auto bd :s->data)
        {
            if (bd->qual != ObjBrowseInfo::eExternal || !s->globalCount)
                if (!Insert(s->index, bd))
                    return false;
        }
    }
    End();
    return true;
}
bool BRCWriter::WriteUsageData(Symbols &syms)
{
    Begin();
    for (auto sym : syms)
    {
        SymData *s = sym.second;
        for (auto bd : s->usages)
        {
            if (bd->qual != ObjBrowseInfo::eExternal || !s->globalCount)
                if (!Insert(s->index, bd, true))
                    return false;
        }
    }
    End();
    return true;
}
bool BRCWriter::WriteJumpTable(Symbols &syms)
{
    Begin();
    for (auto sym : syms)
    {
        SymData *s = sym.second;
        if (s->data.size())
        {
            BrowseData *gl = nullptr, *ex = nullptr;
            for (auto b : s->data)
            {
                if (b->type == ObjBrowseInfo::eFuncStart)
                {
                    if (b->qual == ObjBrowseInfo::eExternal)
                    {
                        ex = b;
                    }
                    else
                    {
                        gl = b;
                    }
                }
            }
            if (!gl)
                gl = ex;
            if (gl)
            {
                int st = gl->startLine;
                int end = gl->funcEndLine;
                if (!Insert(fileMap[gl->fileIndex], st, end, s->index))
                    return false;
            }
        }
    }
    End();
    return true;
}
bool BRCWriter::write()
{
    Symbols &syms = loader.GetSymbols();    

    bool ok = DBOpen((char *)outputFile.c_str());

    if (ok)
    {
        ok = false;
//        if (Begin())
            if (WriteFileList())
                if (WriteDictionary(syms))
                    if (WriteMapping(syms))
                        if (WriteLineData(syms))
                            if (WriteUsageData(syms))
                                if (WriteJumpTable(syms))
                                   ok = true;
//        if (!End())
            ok = false;
    }
    if (dbPointer)
        sqlite3_close(dbPointer);
    return ok;
}
