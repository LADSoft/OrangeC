/*
    Software License Agreement (BSD License)
    
    Copyrigh (c) 1997-2016, David Lindauer, (LADSoft).
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

*/
#include "compiler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

#define TRUE 1
#define FALSE 0

#define STRINGVERSION "121"

#define DBVersion atoi(STRINGVERSION)

#define STRINGIZE(x) #x


void ccLoadIdsFromNameTable(char *tabname, HASHTABLE *table);

typedef struct _symid
{
    char *name;
    sqlite3_int64 id;
} SYMID;
static int version_ok;
char *tables= 
{
    "BEGIN; "
    "CREATE TABLE ccPropertyBag ("
    " property VARCHAR(100)"
    " ,value VARCHAR(200)"
    " );"
    "CREATE TABLE Names ("
    " id INTEGER PRIMARY KEY AUTOINCREMENT"
    " ,name VARCHAR(200) UNIQUE"
    " );"
    "CREATE TABLE FileNames ("
    " id INTEGER PRIMARY KEY AUTOINCREMENT"
    " ,name VARCHAR(260) UNIQUE"
    " ,fileDate DATE"
    " );"
    "CREATE TABLE SymbolTypes ("
    " fileid INTEGER"
    " ,mainid INTEGER"
    " ,symbolid INTEGER"
    " ,startline INTEGER"
    " ,endline INTEGER"
    " ,type INTEGER"
    " );"
    "CREATE TABLE LineData ("
    " fileId INTEGER"
    " ,mainId INTEGER"
    " ,lines INTEGER"
    " ,data BLOB"
    " );"
    "CREATE INDEX LDIndex ON LineData(fileId, mainId);"
    "CREATE TABLE LineNumbers ("
    "  id INTEGER PRIMARY KEY AUTOINCREMENT"
    " ,symbolId INTEGER"
    " ,typeId INTEGER"
    " ,fileId INTEGER"
    " ,indirectCount INTEGER"
    " ,structId INTEGER"
    " ,mainId INTEGER"
    " ,startLine INTEGER"
    " ,endLine INTEGER"
    " ,altendLine INTEGER"
    " ,flags INTEGER"
    " ,FOREIGN KEY (symbolId) REFERENCES Names(id)"
    " ,FOREIGN KEY (typeId) REFERENCES Names(id)"
    " ,FOREIGN KEY (fileId) REFERENCES FileNames(id)"
    " ,FOREIGN KEY (mainId) REFERENCES FileNames(id)"
    " );"
    "CREATE INDEX LNIndex ON LineNumbers(symbolId, mainId);"
    "CREATE TABLE GlobalArgs ("
    " lineId INTEGER"
    " ,mainId INTEGER"
    " ,symbolId INTEGER"
    " ,typeId INTEGER"
    " ,fieldOrder INTEGER"
    " ,FOREIGN KEY (lineId) REFERENCES LineNumbers(id)"
    " ,FOREIGN KEY (mainId) REFERENCES FileNames(id)"
    " ,FOREIGN KEY (typeId) REFERENCES Names(id)"
    " );"
    "CREATE INDEX GAIndex ON GlobalArgs(lineId, symbolId);"
    "CREATE TABLE StructNames ("
    " id INTEGER PRIMARY KEY AUTOINCREMENT"
    " ,name VARCHAR(200) UNIQUE"
    ");"
    "CREATE TABLE MemberNames ("
    " id INTEGER PRIMARY KEY AUTOINCREMENT"
    " ,name VARCHAR(200) UNIQUE"
    " );"
    "CREATE TABLE StructFields ("
    " id INTEGER PRIMARY KEY AUTOINCREMENT"
    " ,nameid INTEGER"
    " ,symbolId INTEGER"
    " ,typeId INTEGER"
    " ,indirectCount INTEGER"
    " ,structId INTEGER"        // reference back to this...
    " ,fileId INTEGER"
    " ,mainId INTEGER"
    " ,flags INTEGER"
    " ,fieldOrder INTEGER"
    " ,FOREIGN KEY (nameId) REFERENCES StructNames(id)"
    " ,FOREIGN KEY (symbolId) REFERENCES Names(id)"
    " ,FOREIGN KEY (typeId) REFERENCES Names(id)"
    " ,FOREIGN KEY (fileId) REFERENCES FileNames(id)"
    " ,FOREIGN KEY (mainId) REFERENCES FileNames(id)"
    " );"
    "CREATE INDEX SFIndex ON StructFields(nameId,mainId, fileId, symbolId);"
    "CREATE TABLE MethodArgs ("
    " structid INTEGER"
    " ,mainId INTEGER"
    " ,symbolId INTEGER"
    " ,typeid INTEGER"
    " ,fieldOrder INTEGER"
    " ,FOREIGN KEY (structid) REFERENCES StructFields(id)"
    " ,FOREIGN KEY (mainId) REFERENCES FileNames(id)"
    " ,FOREIGN KEY (typeid) REFERENCES Names(id)"
    " );"
    "CREATE TABLE CPPNameMapping ("
    " simpleId INTEGER"
    " ,complexId INTEGER"
    " ,mainid INTEGER"
    " ,FOREIGN KEY (mainid) REFERENCES FileNames(id)"
    " );"
    "CREATE TABLE NameSpaceData ("
    " mainid INTEGER"
    " ,fileid INTEGER"
    " ,symbolid INTEGER"
    " ,startline INTEGER"
    " ,endline INTEGER"
    " ,FOREIGN KEY (mainid) REFERENCES FileNames(id)"
    " ,FOREIGN KEY (symbolId) REFERENCES Names(id)"
    " );"
    "CREATE TABLE UsingData ("
    " mainid INTEGER"
    " ,fileid INTEGER"
    " ,symbolid INTEGER"
    " ,parentid INTEGER"
    " ,startline INTEGER"
    " ,FOREIGN KEY (mainid) REFERENCES FileNames(id)"
    " ,FOREIGN KEY (symbolId) REFERENCES Names(id)"
    " );"
    "INSERT INTO ccPropertyBag (property, value)"
    " VALUES (\"ccVersion\", " STRINGVERSION ");"
    "COMMIT; "
} ;
static char *deletion =
{
    "DELETE FROM SymbolTypes WHERE mainId = %d;"
    "DELETE FROM MethodArgs WHERE mainId = %d;"
    "DELETE FROM GlobalArgs WHERE mainId = %d;"
    "DELETE FROM StructFields WHERE mainId = %d;"
    "DELETE FROM LineNumbers WHERE mainId = %d;"
    "DELETE FROM LineData WHERE mainId = %d;"
    "DELETE FROM CPPNameMapping WHERE mainId = %d;"
    "DELETE FROM NameSpaceData WHERE mainId = %d;"
    "DELETE FROM UsingData WHERE mainId = %d;"
};
static sqlite3 *dbPointer;
static sqlite3_stmt *whndln;
static sqlite3_stmt *shndln;
static HASHTABLE *listn;
static sqlite3_stmt *whndlm;
static sqlite3_stmt *shndlm;
static HASHTABLE *listm;
static sqlite3_stmt *whndls;
static sqlite3_stmt *shndls;
static HASHTABLE *lists;
static sqlite3_stmt *whndlf;
static sqlite3_stmt *shndlf;
static HASHTABLE *listf;

static HASHTABLE *map;

void ccReset(void)
{
    whndln = whndlm = whndls =whndlf = NULL;
    shndln = shndlm = shndls =shndlf = NULL;
    listn = CreateHashTable(32000);
    listm = CreateHashTable(32000);
    lists = CreateHashTable(32000);
    listf = CreateHashTable(32000);
    ccLoadIdsFromNameTable("Names", listn);
    ccLoadIdsFromNameTable("MemberNames", listm);
    ccLoadIdsFromNameTable("StructNames", lists);
    ccLoadIdsFromNameTable("FileNames", listf);

    map = CreateHashTable(32000);

}
static void unregister(void)
{
    if (dbPointer)
    {
#ifdef TEST
        printf("closing...\n");
#endif
        sqlite3_close(dbPointer);
        dbPointer = NULL;
    }
}
static int ccallback()
{
    return 0;
}
static int create_exec( char *str)
{
    char *zErrMsg  = 0;
    int rv = FALSE;
    int rc = sqlite3_exec(dbPointer, str, ccallback, 0, &zErrMsg);
    if( rc!=SQLITE_OK )
    {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    else
    {
        rv = TRUE;
    }
    return rv;
}
static int verscallback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    if (argc == 1)
    {
        if (atoi(argv[0]) >= DBVersion)
            version_ok = TRUE;
    }
    return 0;
}
static int checkDb(void)
{
    char *zErrMsg  = 0;
    int rv = FALSE;
    int rc = sqlite3_exec(dbPointer, "SELECT value FROM ccPropertyBag WHERE property = \"ccVersion\"", 
                          verscallback, 0, &zErrMsg);
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
static int createTables(void)
{
    int rv = TRUE;
    if (!create_exec(tables))
    {
        rv = FALSE;
    }
    return rv;
}
int ccDBOpen(char *name)
{
    int rv = FALSE;
    if (sqlite3_open_v2(name, &dbPointer,SQLITE_OPEN_READWRITE, NULL) == SQLITE_OK && checkDb())
    {
#ifdef TEST
        printf("checkdb...");
#endif
        char *zErrMsg  = 0;
        int rc = sqlite3_exec(dbPointer, "PRAGMA journal_mode = OFF; PRAGMA synchronous = OFF;PRAGMA temp_store = MEMORY;", 
                              verscallback, 0, &zErrMsg);
        if( rc!=SQLITE_OK )
        {
          fprintf(stderr, "SQL error: %s\n", zErrMsg);
          sqlite3_free(zErrMsg);
        }
        rv = TRUE;
    }
    else 
    {
        if (dbPointer)
            sqlite3_close(dbPointer);
        dbPointer = NULL;
        remove(name);
        if (sqlite3_open_v2(name, &dbPointer, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL) == SQLITE_OK)
        {
#ifdef TEST
            printf("createdb...");
#endif
            rv = createTables();
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
    atexit(unregister);
    return rv;
}
int ccBegin(void)
{
    int rv = TRUE;
    if (!create_exec("BEGIN"))
    {
        rv = FALSE;
    }
    return rv;
}
int ccEnd(void )
{
    int rv = TRUE;
    if (!create_exec("COMMIT"))
    {
        rv = FALSE;
    }
    return rv;
}
int ccDBDeleteForFile(sqlite3_int64 id)
{
    int rv = TRUE;
    char buf[1000];
    sprintf(buf, deletion, (int)id, (int)id, (int)id, (int)id, (int)id, (int)id, (int)id, (int)id, (int)id);
    if (!create_exec(buf))
    {
        rv = FALSE;
    }
    return rv;
}
void ccLoadIdsFromNameTable(char *tabname, HASHTABLE *table)
{
    static char *query = "SELECT name, id FROM %s";
    int rc = SQLITE_OK;
    char qbuf[256];
    SYMID *v;
    sqlite3_stmt *handle;
    sprintf(qbuf, query, tabname);
    rc = sqlite3_prepare_v2(dbPointer, qbuf, strlen(qbuf)+1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        BOOLEAN done = FALSE;
        while (!done)
        {
            switch(rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    done = TRUE;
                    break;
                case SQLITE_ROW:
                    v = (SYMID *)Alloc(sizeof(SYMID));
                    v->name = litlate((char *)sqlite3_column_text(handle, 0));
                    v->id = sqlite3_column_int64(handle, 1);
                    AddName((SYMBOL *)v, table);
                    rc = SQLITE_OK;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
}
static int ccSelectIdFromNameTable( sqlite3_stmt **shndl, char *name, char *tabname, sqlite3_int64 *id, HASHTABLE *table)
{
    static char *query = "SELECT id FROM %s WHERE name = ?";
    int rc = SQLITE_OK;
    HASHREC **p = LookupName(name, table);
    SYMID *v;
    if (p)
    {
        *id = ((SYMID *)((*p)->p))->id;
        return rc;
    }
    /*
    if (!*shndl)
    {
        char qbuf[256];
        sprintf(qbuf, query, tabname);
        rc = sqlite3_prepare_v2(dbPointer, qbuf, strlen(qbuf)+1, shndl, NULL);
    }
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(*shndl);
        sqlite3_bind_text(*shndl, 1, name, strlen(name), SQLITE_STATIC);
        while (!done)
        {
            switch(rc = sqlite3_step(*shndl))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    done = TRUE;
                    break;
                case SQLITE_ROW:
                    v = (SYMID *)Alloc(sizeof(SYMID));
                    v->name = name;
                    *id = sqlite3_column_int64(*shndl, 0);
                    v->id = *id;
                    AddName((SYMBOL *)v, table);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
    }
    return rc;
    */
    return SQLITE_ERROR;
}
static int ccWriteNameInTable( sqlite3_stmt **whndl, sqlite3_stmt **shndl, char *name, char *tabname, sqlite3_int64 *id, HASHTABLE *table)
{
    static char *query = "INSERT INTO %s (name) VALUES (?)";
    int rc;
    rc = ccSelectIdFromNameTable(shndl, name, tabname, id, table);
    if (rc != SQLITE_OK)
    {
        rc = SQLITE_OK;
        if (!*whndl)
        {
            char qbuf[256];
            sprintf(qbuf, query, tabname);
            rc = sqlite3_prepare_v2(dbPointer, qbuf, strlen(qbuf)+1, whndl, NULL);
        }
        if (rc == SQLITE_OK)
        {
            int done = FALSE;
            sqlite3_reset(*whndl);
            sqlite3_bind_text(*whndl, 1, name, strlen(name), SQLITE_STATIC);
            while (!done)
            {
                switch(rc = sqlite3_step(*whndl))
                {
                    case SQLITE_BUSY:
                        done = TRUE;
                        break;
                    case SQLITE_DONE:
                        rc = SQLITE_OK;
                        done = TRUE;
                        break;
                    default:
                        done = TRUE;
                        break;
                }
            }                        
        }
        if (rc == SQLITE_OK)
        {
            SYMID *v = (SYMID *)Alloc(sizeof(SYMID));
            v->name = name;
            *id = sqlite3_last_insert_rowid(dbPointer);
            v->id = *id;
            AddName((SYMBOL *)v, table);
            
        }
    }
    return rc;
}
static int ccWriteMap( sqlite_int64 smpl_id, sqlite_int64 cplx_id, sqlite_int64 main_id)
{
    char id[256];
    
    static char *query = "INSERT INTO CPPNameMapping (simpleId, complexId, mainid) VALUES (?, ?, ?)";
    int rc = SQLITE_OK;
    static sqlite3_stmt *handle, *handle1;
    if (!handle)
        rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, NULL);
    sprintf(id, "%d,%d", (int)smpl_id, (int)cplx_id);
    if (!LookupName(id, map))
    {
        int done = FALSE;
        SYMID *v = (SYMID *)Alloc(sizeof(SYMID));
        v->name = litlate(id);
        AddName((SYMBOL *)v, map);
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_int64(handle, 1, smpl_id);
        sqlite3_bind_int64(handle, 2, cplx_id);
        sqlite3_bind_int64(handle, 3, main_id);
        while (!done)
        {
            switch(rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    done = TRUE;
                    rc = SQLITE_OK;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
    }
    return rc;
}
int PushCPPNameMapping(char *name, sqlite_int64 cplx_id, sqlite_int64 main_id)
{
    int rc = SQLITE_OK;
    if (strchr(name, '@'))
    {
        int i;
        int first = 0;
        int size = strlen(name);
        int last = size;
        int nested = 0;
        for (i =0; i < size; i++)
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
        if (last != size)
        {
            char buf[2048];
            sqlite_int64 smpl_id;
            memcpy(buf, name+first, last - first);
            buf[last-first] =0;
            if (ccWriteName(buf, &smpl_id, main_id))
            {
                
                rc = ccWriteMap(smpl_id, cplx_id, main_id);   
                if (rc == SQLITE_OK)
                {
                    if (first != 0)
                    {
                        memcpy(buf, name, last);
                        buf[last] = 0;
                        if (ccWriteName(buf, &smpl_id, main_id))
                            rc = ccWriteMap(smpl_id, cplx_id, main_id);   
                    }
                }
            }
        }
    }
    return rc;
}
int ccWriteName( char *name, sqlite_int64 *id, sqlite_int64 main_id)
{
    int rc = ccWriteNameInTable(&whndln, &shndln, name, "Names", id, listn) == SQLITE_OK;
    if (rc)
    {
        rc = PushCPPNameMapping(name, *id, main_id) == SQLITE_OK;
    }
    return rc;
}
int ccWriteMemberName( char *name, sqlite_int64 *id)
{
    return ccWriteNameInTable(&whndlm, &shndlm, name, "MemberNames", id, listm) == SQLITE_OK;
}
int ccWriteStructName( char *name, sqlite_int64 *id)
{
    return ccWriteNameInTable(&whndls, &shndls, name, "StructNames", id, lists) == SQLITE_OK;
}
int ccWriteFileName( char *name, sqlite_int64 *id)
{
    char buf[260], *p = buf;
    HASHREC **q;
    while (*name)
        *p++ = tolower(*name++);
    *p = 0;
    q = LookupName(buf, listf);
    if (q)
    {
        *id = ((SYMID *)((*q)->p))->id;
        return TRUE;
    }
    name = litlate(buf);
    return ccWriteNameInTable(&whndlf, &shndlf, name, "FileNames", id, listf) == SQLITE_OK;
}
int ccWriteFileTime( char *name, int time, sqlite3_int64 *id)
{
    int rc = SQLITE_OK;
    static char *query = "UPDATE FileNames SET fileDate = ? WHERE id = ?";
    int rv = ccWriteFileName(name, id);
    if (rv)
    {
        static sqlite3_stmt *handle;
        if (!handle)
        {
            rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, NULL);
        }
        if (rc == SQLITE_OK)
        {
            int done = FALSE;
            sqlite3_reset(handle);
            sqlite3_bind_int(handle, 1, time);
            sqlite3_bind_int64(handle, 2, *id);
            while (!done)
            {
                switch(rc = sqlite3_step(handle))
                {
                    case SQLITE_BUSY:
                        done = TRUE;
                        break;
                    case SQLITE_DONE:
                        rc = SQLITE_OK;
                        done = TRUE;
                        break;
                    default:
                        done = TRUE;
                        break;
                }
            }                        
        }
    }
    else
    {
        rc = SQLITE_ERROR;
    }
    return rc == SQLITE_OK;
}
int ccWriteLineNumbers( char *symname, char *typename, char *filename, int indirectCount, 
                       sqlite_int64 struct_id, sqlite_int64 main_id, int start, int end, int altend, int flags, sqlite_int64 *id)
{
    int rc = SQLITE_ERROR;
    sqlite3_int64 sym_id, type_id, file_id;
    if (ccWriteName(symname, &sym_id, main_id) && ccWriteName(typename, &type_id, main_id) &&
        ccWriteFileName(filename, &file_id))
    {
        static char *query = "INSERT INTO LineNumbers (symbolId, typeId, fileId, indirectCount, structId, mainId, startLine, endLine, altendline, flags)"
                             " VALUES (?,?,?,?,?,?,?,?,?,?)";
        static sqlite3_stmt *handle;
        rc = SQLITE_OK;
        if (!handle)
        {
            rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, NULL);
        }
        if (rc == SQLITE_OK)
        {
            int done = FALSE;
            
            sqlite3_reset(handle);
            sqlite3_bind_int64(handle, 1, sym_id);
            sqlite3_bind_int64(handle, 2, type_id);
            sqlite3_bind_int64(handle, 3, file_id);
            sqlite3_bind_int(handle, 4, indirectCount);
            sqlite3_bind_int64(handle, 5, struct_id);
            sqlite3_bind_int64(handle, 6, main_id);
            sqlite3_bind_int(handle, 7, start);
            sqlite3_bind_int(handle, 8, end);
            sqlite3_bind_int(handle, 9, altend);
            sqlite3_bind_int(handle, 10, flags);
            while (!done)
            {
                switch(rc = sqlite3_step(handle))
                {
                    case SQLITE_BUSY:
                        done = TRUE;
                        break;
                    case SQLITE_DONE:
                        rc = SQLITE_OK;
                        done = TRUE;
                        break;
                    default:
                        done = TRUE;
                        break;
                }
            }                        
        }
        if (rc == SQLITE_OK)
        {
            *id = sqlite3_last_insert_rowid(dbPointer);
        }
    }
    return rc == SQLITE_OK;
}
int ccWriteGlobalArg( sqlite_int64 line_id, sqlite_int64 main_id, char *symname, char *typename, int *order)
{
    int rc = SQLITE_ERROR;
    sqlite3_int64 type_id, symbol_id;
    if (ccWriteName(symname, &symbol_id,  main_id) && ccWriteName(typename, &type_id, main_id))
    {
        static char *query = "INSERT INTO GlobalArgs (lineId, mainid, symbolId, typeId, fieldOrder)"
                             " VALUES (?,?,?,?,?)";
        static sqlite3_stmt *handle;
        rc = SQLITE_OK;
        if (!handle)
        {
            rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, NULL);
        }
        if (rc == SQLITE_OK)
        {
            int done = FALSE;
            sqlite3_reset(handle);
            sqlite3_bind_int64(handle, 1, line_id);
            sqlite3_bind_int64(handle, 2, main_id);
            sqlite3_bind_int64(handle, 3, symbol_id);
            sqlite3_bind_int64(handle, 4, type_id);
            sqlite3_bind_int64(handle, 5, (*order)++);
            while (!done)
            {
                switch(rc = sqlite3_step(handle))
                {
                    case SQLITE_BUSY:
                        done = TRUE;
                        break;
                    case SQLITE_DONE:
                        rc = SQLITE_OK;
                        done = TRUE;
                        break;
                    default:
                        done = TRUE;
                        break;
                }
            }                        
        }        
    }
    return rc == SQLITE_OK;
}
int ccWriteStructField( sqlite3_int64 name_id, char *symname, char *typename, 
                       int indirectCount, sqlite3_int64 struct_id, 
                       sqlite3_int64 file_id, sqlite_int64 main_id, int flags, int *order, sqlite_int64 *id)
{
    int rc = SQLITE_ERROR;
    sqlite3_int64 sym_id, type_id;
    if (ccWriteMemberName(symname, &sym_id) && 
        ccWriteName(typename, &type_id, main_id))
    {
        static char *query = "INSERT INTO StructFields (nameId, symbolId, typeId, indirectCount, structId, fileId, mainId, flags, fieldOrder)"
                             " VALUES (?,?,?,?,?,?,?,?,?)";
        static sqlite3_stmt *handle;
        rc = SQLITE_OK;
        if (!handle)
        {
            rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, NULL);
        }
        if (rc == SQLITE_OK)
        {
            int done = FALSE;
            sqlite3_reset(handle);
            sqlite3_bind_int64(handle, 1, name_id);
            sqlite3_bind_int64(handle, 2, sym_id);
            sqlite3_bind_int64(handle, 3, type_id);
            sqlite3_bind_int(handle, 4, indirectCount);
            sqlite3_bind_int64(handle, 5, struct_id);
            sqlite3_bind_int64(handle, 6, file_id);
            sqlite3_bind_int64(handle, 7, main_id);
            sqlite3_bind_int64(handle, 8, flags);
            sqlite3_bind_int(handle, 9, (*order)++);
            while (!done)
            {
                switch(rc = sqlite3_step(handle))
                {
                    case SQLITE_BUSY:
                        done = TRUE;
                        break;
                    case SQLITE_DONE:
                        rc = SQLITE_OK;
                        done = TRUE;
                        break;
                    default:
                        done = TRUE;
                        break;
                }
            }                        
        }
        if (rc == SQLITE_OK)
        {
            *id = sqlite3_last_insert_rowid(dbPointer);
        }
    }
    return rc == SQLITE_OK;
}
int ccWriteMethodArg( sqlite_int64 struct_id, char *typename, int *order, sqlite3_int64 main_id)
{
    int rc = SQLITE_ERROR;
    sqlite3_int64 type_id;
    if (ccWriteName(typename, &type_id, main_id))
    {
        static char *query = "INSERT INTO MethodArgs (structId, typeId, fieldOrder)"
                             " VALUES (?,?,?)";
        static sqlite3_stmt *handle;
        rc = SQLITE_OK;
        if (!handle)
        {
            rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, NULL);
        }
        if (rc == SQLITE_OK)
        {
            int done = FALSE;
            sqlite3_reset(handle);
            sqlite3_bind_int64(handle, 1, struct_id);
            sqlite3_bind_int64(handle, 2, type_id);
            sqlite3_bind_int64(handle, 3, (*order)++);
            while (!done)
            {
                switch(rc = sqlite3_step(handle))
                {
                    case SQLITE_BUSY:
                        done = TRUE;
                        break;
                    case SQLITE_DONE:
                        rc = SQLITE_OK;
                        done = TRUE;
                        break;
                    default:
                        done = TRUE;
                        break;
                }
            }                        
        }
        
    }
    return rc == SQLITE_OK;
}
int ccWriteLineData(sqlite_int64 file_id, sqlite_int64 main_id, char *data, int len, int lines)
{
    sqlite3_int64 type_id;
    static char *query = "INSERT INTO LineData (fileId, mainId, lines, data)"
                         " VALUES (?,?,?,?)";
    static sqlite3_stmt *handle;
    int rc = SQLITE_OK;
    if (!handle)
    {
        rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, NULL);
    }
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        sqlite3_reset(handle);
        sqlite3_bind_int64(handle, 1, file_id);
        sqlite3_bind_int64(handle, 2, main_id);
        sqlite3_bind_int64(handle, 3, lines);
        sqlite3_bind_blob(handle, 4, data, len, SQLITE_STATIC);
        while (!done)
        {
            switch(rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }                        
    }
    return rc == SQLITE_OK;
}
int ccWriteSymbolType( char *symname, sqlite3_int64 file_id, char *declFile, int startLine, int endLine, int type)
{
    int rc = SQLITE_ERROR;
    sqlite3_int64 sym_id, localFileId;
    if (ccWriteName(symname, &sym_id, file_id) && ccWriteFileName( declFile, &localFileId))
    {
        static char *query = "INSERT INTO SymbolTypes (mainid, fileid, symbolid, startline, endline, type)"
                             " VALUES (?,?,?,?,?,?)";
        static sqlite3_stmt *handle;
        rc = SQLITE_OK;
        if (!handle)
        {
            rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, NULL);
        }
        if (rc == SQLITE_OK)
        {
            int done = FALSE;
            
            sqlite3_reset(handle);
            sqlite3_bind_int64(handle, 1, file_id);
            sqlite3_bind_int64(handle, 2, localFileId);
            sqlite3_bind_int64(handle, 3, sym_id);
            sqlite3_bind_int64(handle, 4, startLine);
            sqlite3_bind_int64(handle, 5, endLine);
            sqlite3_bind_int64(handle, 6, type);
            while (!done)
            {
                switch(rc = sqlite3_step(handle))
                {
                    case SQLITE_BUSY:
                        done = TRUE;
                        break;
                    case SQLITE_DONE:
                        rc = SQLITE_OK;
                        done = TRUE;
                        break;
                    default:
                        done = TRUE;
                        break;
                }
            }                        
        }
    }
    return rc == SQLITE_OK;
}
int ccWriteNameSpaceEntry(char *symname, sqlite_int64 file_id, char *filename, int startline, int endline)
{
    int rc = SQLITE_ERROR;
    sqlite3_int64 sym_id, localFileId;
    if (ccWriteName(symname, &sym_id, file_id) && ccWriteFileName( filename, &localFileId))
    {
        static char *query = "INSERT INTO NameSpaceData (mainid, fileid, symbolid, startline, endline)"
                             " VALUES (?,?,?,?,?)";
        static sqlite3_stmt *handle;
        rc = SQLITE_OK;
        if (!handle)
        {
            rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, NULL);
        }
        if (rc == SQLITE_OK)
        {
            int done = FALSE;
            
            sqlite3_reset(handle);
            sqlite3_bind_int64(handle, 1, file_id);
            sqlite3_bind_int64(handle, 2, localFileId);
            sqlite3_bind_int64(handle, 3, sym_id);
            sqlite3_bind_int64(handle, 4, startline);
            sqlite3_bind_int64(handle, 5, endline);
            while (!done)
            {
                switch(rc = sqlite3_step(handle))
                {
                    case SQLITE_BUSY:
                        done = TRUE;
                        break;
                    case SQLITE_DONE:
                        rc = SQLITE_OK;
                        done = TRUE;
                        break;
                    default:
                        done = TRUE;
                        break;
                }
            }                        
        }
    }
    return rc == SQLITE_OK;
}
int ccWriteUsingRecord(char *symname, char *parentname, char *filename, int startline, sqlite_int64 file_id)
{
    sqlite3_int64 sym_id, parent_id= -1, localFileId;
    int rc = SQLITE_ERROR;
    if (ccWriteName(symname, &sym_id, file_id) && (!parentname[0] || ccWriteName(parentname, &parent_id, file_id)) && ccWriteFileName( filename, &localFileId))
    {
        static char *query = "INSERT INTO UsingData (mainid, fileid, symbolid, parentid, startline)"
                             " VALUES (?,?,?,?,?)";
        static sqlite3_stmt *handle;
        rc = SQLITE_OK;
        if (!handle)
        {
            rc = sqlite3_prepare_v2(dbPointer, query, strlen(query)+1, &handle, NULL);
        }
        if (rc == SQLITE_OK)
        {
            int done = FALSE;
            
            sqlite3_reset(handle);
            sqlite3_bind_int64(handle, 1, file_id);
            sqlite3_bind_int64(handle, 2, localFileId);
            sqlite3_bind_int64(handle, 3, sym_id);
            sqlite3_bind_int64(handle, 4, parent_id);
            sqlite3_bind_int64(handle, 5, startline);
            while (!done)
            {
                switch(rc = sqlite3_step(handle))
                {
                    case SQLITE_BUSY:
                        done = TRUE;
                        break;
                    case SQLITE_DONE:
                        rc = SQLITE_OK;
                        done = TRUE;
                        break;
                    default:
                        done = TRUE;
                        break;
                }
            }                        
        }
    }
    return rc == SQLITE_OK;
}
#ifdef TEST
static int writeTempRecords(void)
{
    sqlite3_int64 a, file, struc;
    int rv = TRUE;
    int order;
    printf("%d\n", ccBegin());
    rv &= ccWriteName("n1", &a);
    printf("%d %ld\n",rv, a);
    rv &= ccWriteName("n2", &a);
    printf("%d %ld\n",rv, a);
    rv &= ccWriteName("n3", &a);
    printf("%d %ld\n",rv, a);
    rv &= ccWriteName("t1", &a);
    printf("%d %ld\n",rv, a);
    rv &= ccWriteName("t2", &a);
    printf("%d %ld\n",rv, a);
    rv &= ccWriteName("t3", &a);
    printf("%d %ld\n",rv, a);
    rv &= ccWriteStructName("s1", &a);
    printf("%d %ld\n",rv, a);
    rv &= ccWriteStructName("s2", &a);
    printf("%d %ld\n",rv, a);
    rv &= ccWriteStructName("s3", &a);
    printf("%d %ld\n",rv, a);
    rv &= ccWriteFileName("f1", &a);
    printf("%d %ld\n",rv, a);
    rv &= ccWriteFileName("f2", &a);
    printf("%d %ld\n",rv, a);
    rv &= ccWriteFileName("f3", &a);
    printf("%d %ld\n",rv, a);
    rv &= ccWriteFileName("f4", &a);
    printf("%d %ld\n",rv, a);
    rv &= ccWriteFileTime("f5", 12345678);
    printf("%d\n",rv);
    rv &= ccWriteFileTime("f1", 87654321);
    printf("%d\n",rv);
    printf("line numbers now\n");
    rv &= ccWriteLineNumbers("n1", "t1", "hi.txt", 2, 1 , 3, &a);
    printf("%d\n",rv);
    order = 1;
    rv &= ccWriteGlobalArg(a, "t1", &order);
    printf("%d\n",rv);
    rv &= ccWriteGlobalArg(a, "t2", &order);
    printf("%d\n",rv);
    rv &= ccWriteGlobalArg(a, "t1", &order);
    printf("%d\n",rv);
    rv &= ccWriteGlobalArg(a, "t17", &order);
    printf("%d\n",rv);
    rv &= ccWriteLineNumbers("n1", "t1", "f1", 0, 6 , 7, &a);
    printf("%d\n",rv);
    order = 1;
    rv &= ccWriteGlobalArg(a, "t1", &order);
    printf("%d\n",rv);
    rv &= ccWriteGlobalArg(a, "t2", &order);
    printf("%d\n",rv);
    rv &= ccWriteGlobalArg(a, "t1", &order);
    printf("%d\n",rv);
    rv &= ccWriteGlobalArg(a, "t17", &order);
    printf("%d\n",rv);
    rv &= ccWriteLineNumbers("n2", "t1", "f3", 1, 6 , 7, &a);
    printf("%d\n",rv);
    printf("structs now\n");
    order = 1;
    rv &= ccWriteStructName("s1", &struc);
    printf("%d\n",rv);
    rv &= ccWriteFileName("f18", &file);
    printf("%d\n",rv);
    rv &= ccWriteStructField(struc, "aa", "t1", file, &order, &a);
    printf("%d\n",rv);
    rv &= ccWriteMethodArg(a, "t1", &order);
    printf("%d\n",rv);
    rv &= ccWriteMethodArg(a, "t2", &order);
    printf("%d\n",rv);
    rv &= ccWriteStructField(struc, "bb", "t2", file, &order, &a);
    printf("%d\n",rv);
    rv &= ccWriteStructField(struc, "cc", "t3", file, &order, &a);
    printf("%d\n",rv);
    rv &= ccWriteStructField(struc, "dd", "t1", file, &order, &a);
    printf("%d\n",rv);
    order = 1;
    rv &= ccWriteStructName("s1", &struc);
    printf("%d\n",rv);
    rv &= ccWriteFileName("hi.txt", &file);
    printf("%d\n",rv);
    rv &= ccWriteStructField(struc, "aa1", "t1", file, &order, &a);
    printf("%d\n",rv);
    rv &= ccWriteStructField(struc, "bb1", "t2", file, &order, &a);
    printf("%d\n",rv);
    rv &= ccWriteStructField(struc, "cc1", "t3", file, &order, &a);
    printf("%d\n",rv);
    rv &= ccWriteStructField(struc, "dd1", "t1", file, &order, &a);
    printf("%d\n",rv);
    rv &= ccWriteMethodArg(a, "t2", &order);
    printf("%d\n",rv);
    rv &= ccWriteMethodArg(a, "t1", &order);
    printf("%d\n",rv);
    printf("%d\n", ccEnd());
    return rv;
        
}
main(int argc, char *argv[])
{
    if (argc > 1)
    {
        if (ccDBOpen(argv[1]))
        {
            printf("success\n");
            if (writeTempRecords())
                printf("records written\n");
            if (argc > 2 && ccDBDeleteForFile("hi.txt"))
                printf("deleted\n");
        }
        else
        {
            printf("failure\n");
        }
    }
    return 0;
}
#endif