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
#include "BRCDictionary.h"
#include "BRCWriter.h"
#include "ObjBrowseInfo.h"
#include <stdio.h>
#include <algorithm>
#define STRINGVERSION "100"

#define DBVersion atoi(STRINGVERSION)

#define JT_NONE 0
#define JT_FUNC 1
#define JT_VAR 2
#define JT_DEFINE 4
#define JT_TYPEDATA 8
#define JT_LOCALDATA 0x10
#define JT_STATIC 0x20
#define JT_GLOBAL 0x40

static int version_ok;

char *BRCWriter::tables= 
{
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
    "CREATE TABLE JumpTable ("
    " symbolId INTEGER"
    " ,fileId INTEGER"
    " ,startLine INTEGER"
    " ,endLine INTEGER"
    " ,FOREIGN KEY (symbolId) REFERENCES Names(id)"
    " );"
    "INSERT INTO brPropertyBag (property, value)"
    " VALUES (\"brVersion\", "STRINGVERSION");"
    "COMMIT; "
} ;
char *BRCWriter::deletion =
{
    "BEGIN;"
    "DELETE FROM Names;"
    "DELETE FROM FileNames;"
    "DELETE FROM LineNumbers;"
    "DELETE FROM JumpTable;"
    "COMMIT;"
    "VACUUM"
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
    if (sqlite3_open_v2(name, &dbPointer,SQLITE_OPEN_READWRITE, NULL) == SQLITE_OK)
    {
#ifdef TEST
        printf("checkdb...");
#endif
        rv = CheckDb();
        if (!rv || !SQLiteExec(deletion))
        {
            sqlite3_close(dbPointer);
            unlink(name);
            goto doCreate;
        }
    }
    else 
    {
        if (dbPointer)
            sqlite3_close(dbPointer);
doCreate:
        dbPointer = NULL;
        if (sqlite3_open_v2(name, &dbPointer, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL) == SQLITE_OK)
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
bool BRCWriter::Insert(std::string fileName, int index)
{
    static char *query = "INSERT INTO FileNames (name) VALUES (?)";
    int rc = SQLITE_OK;
    static sqlite3_stmt *handle;
    if (!handle)
    {
        rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, NULL);
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
        rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, NULL);
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
        rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, NULL);
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
bool BRCWriter::Insert(sqlite3_int64 symIndex, BrowseData *b)
{
    static char *query = "INSERT INTO LineNumbers (type, qual, symbolId, fileId, startLine, charPos, hint)"
                         " VALUES (?,?,?,?,?,?,?)";
    int rc = SQLITE_OK;
    static sqlite3_stmt *handle;
    if (!handle)
    {
        rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, NULL);
    }
    if (rc == SQLITE_OK)
    {
        int done = false;
        sqlite3_reset(handle);
        sqlite3_bind_int(handle, 1, b->type | (b->blockLevel == 0 ? 0x4000 : 0));
        sqlite3_bind_int(handle, 2, b->qual);
        sqlite3_bind_int64(handle, 3, symIndex);
        sqlite3_bind_int64(handle, 4, fileMap[b->fileIndex]);
        sqlite3_bind_int(handle, 5, b->startLine);
        sqlite3_bind_int(handle, 6, b->charPos);
        sqlite3_bind_text(handle, 7, b->hint.c_str(), b->hint.size(), SQLITE_STATIC);
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
char easytolower(char in){
  if(in<='Z' && in>='A')
    return in-('Z'-'z');
  return in;
} 
bool BRCWriter::WriteFileList()
{
    std::map<std::string, int> &unsorted = loader.GetFileNames();
    for (std::map<std::string, int>::iterator it = unsorted.begin(); it != unsorted.end(); ++it)
    {
        std::string myString = it->first;
        std::transform(myString.begin(), myString.end(), myString.begin(), easytolower); 
        if (!Insert(myString.c_str(), it->second))
            return false;
    }
    return true;
}
bool BRCWriter::WriteDictionary(Symbols &syms)
{
    for (Symbols::iterator it = syms.begin(); it != syms.end(); ++it)
    {
        SymData *sym = it->second;
        int type = 0;
        for (BrowseDataset::iterator it = sym->data.begin(); it != sym->data.end(); ++it)
        {
            BrowseData *l = *it;
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
        if (!Insert(it->first.c_str(), type, &sym->index))
            return false;
    }
    return true;
}
bool BRCWriter::WriteLineData(Symbols &syms)
{
    for (Symbols::iterator it = syms.begin(); it != syms.end(); ++it)
    {
        SymData *s = it->second;
        for (BrowseDataset::iterator it = s->data.begin(); it != s->data.end(); ++it)
        {
            BrowseData *bd = *it;
            if (bd->qual != ObjBrowseInfo::eExternal || !s->globalCount)
                if (!Insert(s->index, bd))
                    return false;
        }
    }
    return true;
}
bool BRCWriter::WriteJumpTable(Symbols &syms)
{
    for (Symbols::iterator it = syms.begin(); it != syms.end(); ++it)
    {
        SymData *s = it->second;
        if (s->data.size())
        {
            BrowseData *gl = NULL, *ex = NULL;
            for (BrowseDataset::iterator it1 = s->data.begin(); it1 != s->data.end(); ++it1)
            {
                BrowseData *b = *it1;
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
    return true;
}
bool BRCWriter::write()
{
    Symbols &syms = loader.GetSymbols();    

    bool ok = DBOpen((char *)outputFile.c_str());

    if (ok)
    {
        ok = false;
        if (Begin())
            if (WriteFileList())
                if (WriteDictionary(syms))
                    if (WriteLineData(syms))
                        if (WriteJumpTable(syms))
                            ok = true;
        if (!End())
            ok = false;
    }
    if (dbPointer)
        sqlite3_close(dbPointer);
    return ok;
}
