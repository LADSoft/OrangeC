/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2012, David Lindauer, (LADSoft).
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
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include "codecomp.h"
#include <sys/stat.h>
#include <process.h>
#include <time.h>
#include <ctype.h>
#define FROMIDE

#define DBVersion 120

extern PROJECTITEM *workArea;
extern DWINFO *editWindows;
extern char szInstallPath[];
extern HANDLE editHeap;
extern HANDLE ewSem;
extern HANDLE makeSem;
extern int defaultWorkArea;

static char prm_searchpath[10000] ;
static char sys_searchpath[256] ;

static char pipeName[MAX_PATH];

static char *localEmptyString = "";
static sqlite3 *db;

static int version_ok;

static HANDLE ccEvent;

static BOOL vacuuming;

void DBClose(sqlite3 *db);
char *getcwd( char *__buf, int __buflen ); // can't include dir.h because it defines eof...

void ReloadLineData(char *name)
{
    DWINFO *ptr = editWindows;
    DWINFO info;
    strcpy(info.dwName, name);
    while (ptr)
    {
        if (ptr->active && SendMessage(ptr->self, WM_COMMAND, ID_QUERYHASFILE, (LPARAM)
            &info))
        {
            PostMessage(ptr->dwHandle, EM_LOADLINEDATA, 0, 0);
            InvalidateRect(ptr->dwHandle, 0, 0);
            InvalidateRect(ptr->self, 0, 0);
            break;
        }
        ptr = ptr->next;
    }
}
static int prepare(sqlite3 *db, char *name, int len, sqlite3_stmt** handle, char ** vv)
{
    int rv;
    if (!ccEvent)
        ccEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
    if (MsgWait(ccEvent, 200) != WAIT_OBJECT_0)
        return SQLITE_ERROR;
    rv = sqlite3_prepare_v2(db, name, len, handle, vv);
    if (rv != SQLITE_OK)
        SetEvent(ccEvent);
    return rv;
}
static int finalize(sqlite3_stmt *handle)
{
    int rv = sqlite3_finalize(handle);
    SetEvent(ccEvent);
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
static int busyFunc(void*handle,int times)
{
    static DWORD timer;
    if (times == 0)
    {
        timer = GetTickCount();
        Sleep(0);
        return 1;
    }
    else if (GetTickCount() - timer >= 800)
    {
        return 0;
    }
    else {   
        MSG msg;
        while (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
            ProcessMessage(&msg);
        Sleep(0);
        return 1;
    }
}
static void dbVacuum(void *xx)
{
    char *zErrMsg = NULL;
    sqlite3 *xdb = xx;
    DWINFO *p;
    if (sqlite3_exec(xdb, "PRAGMA journal_mode = OFF; PRAGMA synchronous = OFF;PRAGMA temp_store = MEMORY;",//VACUUM;", 
                          verscallback, 0, &zErrMsg)!=SQLITE_OK )
    {
          sqlite3_free(zErrMsg);
    }            
    vacuuming = FALSE;
    WaitForSingleObject(ewSem, INFINITE);
    p = editWindows;
    while (p)
    {
        if (p->active)
            InvalidateRect(p->dwHandle, 0, 0);
        p = p->next;
    }
    SetEvent(ewSem);
}
void ccDBVacuum(sqlite3 *db)
{
    if (db)
    {
        vacuuming = TRUE;
        _beginthread((BEGINTHREAD_FUNC)dbVacuum, 0, (LPVOID)db);       
    }
}
void ccDBSetDB(sqlite3 *xdb)
{
    db = xdb;
}
sqlite3 *ccDBOpen(PROJECTITEM *pj)
{
    sqlite3 *xdb;
    char *zErrMsg = NULL;
    char name[MAX_PATH], *p;
    strcpy(name, pj->realName);
    p = strrchr(name, '.');
    if (!p)
        p = name + strlen(name);
    strcpy(p, ".ods");
    DBClose(db);
    db = NULL;
    vacuuming = FALSE;
    if (sqlite3_open_v2(name, &xdb,SQLITE_OPEN_READWRITE, NULL) == SQLITE_OK)
    {
        char *zErrMsg = NULL;
        version_ok = FALSE;
        if (sqlite3_exec(xdb, "SELECT value FROM ccPropertyBag WHERE property = \"ccVersion\"", 
                              verscallback, 0, &zErrMsg) != SQLITE_OK || !version_ok)
        {
            sqlite3_free(zErrMsg);
            DBClose(xdb);
            xdb = NULL;
        }
        else
        {
            sqlite3_busy_handler(xdb, busyFunc, NULL);
        }
            
    }
    else
    {
        DBClose(xdb);
        xdb = NULL;
    }
    return xdb;
}
static void DoCompleteCommand(PROJECTITEM *pj)
{
    char *cmd= Lookup("__CODE_COMPLETION", pj, NULL);
    if (cmd)
    {
        PROJECTITEM *project = pj;
        while (project && project->type != PJ_PROJ)
            project = project->parent;
        Execute(cmd, project->realName, ERR_NO_WINDOW);
        free(cmd);
    }
        
}
static int FindCompleteCommand(PROJECTITEM *pj, char *name)
{
    int rv = 0;
    do
    {
        AddSymbolTable(pj, FALSE);
        if (pj->type == PJ_FILE)
        {
            
            if (!xstricmp(name, pj->realName))
            {
                DoCompleteCommand(pj);
                RemoveSymbolTable();
                break;
            }
        }
        else if (pj->children)
        {
              
            FindCompleteCommand(pj->children, name);
        }
        RemoveSymbolTable();
        pj = pj->next;
    } while (pj);
    return rv;
}
static BOOL changed(char *name)
{
    char lcname[MAX_PATH], *p= lcname, *q = name;
    struct stat statbuf;
    static char *query = {
       "SELECT fileDate FROM FileNames"
       "    WHERE fileNames.name = ?;"
    } ;
    BOOL idefile;
    sqlite3_stmt *handle;
    int rc;
    time_t t;
    DWINFO *ptr;
    EDITDATA *e;
    if (vacuuming)
        return FALSE;
    WaitForSingleObject(ewSem, INFINITE);
    ptr = editWindows;
    while (ptr)
    {
        if (ptr->active && !xstricmp(name,ptr->dwName))
        {
            e = ptr->editData;
            break;
        }
        ptr = ptr->next;
    }
    idefile = !!e;
    SetEvent(ewSem);
    if (idefile)
    {
        return TRUE;
    }
    while (*q) *p++=tolower(*q++);
    *p = 0;
    rc = prepare(db, query, strlen(query)+1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_text(handle, 1, lcname, strlen(lcname), SQLITE_STATIC);
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
                    t = sqlite3_column_int(handle, 0);
                    done = TRUE;
                    rc = SQLITE_OK;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        finalize(handle);
        if (rc != SQLITE_OK)
            return TRUE;
        stat(name, &statbuf);
        return statbuf.st_mtime > t;
    }
    return TRUE;
}
void DoParse(char *name)
{
    if (vacuuming)
        return;
    if (workArea && changed(name))
    {
        char dbName[MAX_PATH], *p;
        PROJECTITEM *pj = workArea;
        strcpy(dbName, pj->realName);
        p = strrchr(dbName, '.');
        if (!p)
            p = dbName + strlen(dbName);
        strcpy(p, ".ods");
        if (defaultWorkArea) // this is a convenience so I an use code completion without a project
        {
            char cmd[10000];
            char cwd[MAX_PATH];
            getcwd(cwd, MAX_PATH);
            strcat(cwd,"\\xx");
            sprintf(cmd, "\"%s\\bin\\occpr.exe\" /1 \"-o%s\" /P%s %s", szInstallPath, dbName, pipeName, name);
            Execute(cmd, cwd, ERR_NO_WINDOW);
        }
        else
        {
            CreateTempFileName();
            WaitForSingleObject(makeSem, INFINITE);
            AddRootTables(pj, FALSE);
            AddMakeSymbol("__CODE_COMPLETION_FILE", dbName, TRUE, FALSE);
            AddMakeSymbol("__CODE_COMPLETION_PIPE", pipeName, TRUE, FALSE);
            SetOutputExtensions(pj, TRUE);
            SetOutputNames(pj, TRUE);
            FindCompleteCommand(pj, name);
            ReleaseSymbolTables();
            ReleaseSemaphore(makeSem, 1, NULL);
        }
        ReloadLineData(name);
        if (db == NULL && !vacuuming)
            ccDBSetDB(ccDBOpen(pj));
    }
}
void deleteFileData(char *name)
{
}
static int findLine(char *file, int line, char *name, sqlite3_int64 *line_id, 
                    sqlite3_int64 * struct_id,
                    sqlite_int64 * sym_id, sqlite_int64 *type_id, int * flags, int *indirectCount)
{
    // this query to attempt to find it in the current file
    static char *query = {
        "SELECT Linenumbers.id, structId, symbolId, typeId, flags, indirectCount FROM LineNumbers"
        "    JOIN FileNames ON linenumbers.fileId = fileNames.id"
        "    JOIN Names ON LineNumbers.symbolId = names.id"
        "    WHERE Names.Name=?"
        "        AND fileNames.name = ?"
        "        AND ? >= startLine AND (? <= endLine OR endLine = 0);"
//        "    ORDER BY lineNumbers.startLine ASC;"
    };
    static char *query2 = {
        "SELECT Linenumbers.id, structId, symbolId, typeId, flags, indirectCount FROM LineNumbers"
        "    JOIN FileNames ON linenumbers.mainId = fileNames.id"
        "    JOIN Names ON LineNumbers.symbolId = names.id"
        "    WHERE Names.Name=?"
        "        AND fileNames.name = ?"
        "        AND LineNumbers.mainId != LineNumbers.fileId AND endLine = 0;"
//        "    ORDER BY lineNumbers.startLine ASC;"
    };
    int i, l = strlen(file);
    int rc = SQLITE_OK;
    static sqlite3_stmt *handle;
    for (i=0; i < l; i++)
        file[i] = tolower(file[i]);
    rc = prepare(db, query, strlen(query)+1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_text(handle, 1, name, strlen(name), SQLITE_STATIC);
        sqlite3_bind_text(handle, 2, file, strlen(file), SQLITE_STATIC);
        sqlite3_bind_int(handle, 3, line);
        sqlite3_bind_int(handle, 4, line);
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
                    *line_id = sqlite3_column_int64(handle, 0);
                    *struct_id = sqlite3_column_int64(handle, 1);
                    *sym_id = sqlite3_column_int64(handle, 2);
                    *type_id = sqlite3_column_int64(handle, 3);
                    *flags = sqlite3_column_int(handle, 4);
                    *indirectCount = sqlite3_column_int(handle, 5);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        finalize(handle);
        if (rc != SQLITE_OK)
        {
            sqlite3_stmt *handle2;
            rc = prepare(db, query2, strlen(query2)+1, &handle2, NULL);
            if (rc == SQLITE_OK)
            {
                int done = FALSE;
                rc = SQLITE_DONE;
                sqlite3_reset(handle2);
                sqlite3_bind_text(handle2, 1, name, strlen(name), SQLITE_STATIC);
                sqlite3_bind_text(handle2, 2, file, strlen(file), SQLITE_STATIC);
                while (!done)
                {
                    switch(rc = sqlite3_step(handle2))
                    {
                        case SQLITE_BUSY:
                            done = TRUE;
                            break;
                        case SQLITE_DONE:
                            done = TRUE;
                            break;
                        case SQLITE_ROW:
                            *line_id = sqlite3_column_int64(handle2, 0);
                            *struct_id = sqlite3_column_int64(handle2, 1);
                            *sym_id = sqlite3_column_int64(handle2, 2);
                            *type_id = sqlite3_column_int64(handle2, 3);
                            *flags = sqlite3_column_int(handle2, 4);
                            *indirectCount = sqlite3_column_int(handle2, 5);
                            rc = SQLITE_OK;
                            done = TRUE;
                            break;
                        default:
                            done = TRUE;
                            break;
                    }
                }
                finalize(handle2);
            }
        }
    }
    return rc == SQLITE_OK;
}
static int findLine2(char *file, char *name, sqlite3_int64 *line_id, 
                    sqlite3_int64 * struct_id,
                    sqlite_int64 * sym_id, sqlite_int64 *type_id, int * flags)
{
    // this query to attempt to find it in the current file
    static char *query = {
        "SELECT Linenumbers.id, structId, symbolId, typeId, flags FROM LineNumbers"
        "    JOIN FileNames ON linenumbers.fileId = fileNames.id"
        "    JOIN Names ON LineNumbers.symbolId = names.id"
        "    WHERE Names.Name=?"
        "        AND fileNames.name = ? AND endLine = 0 AND flags=1"
        "    ORDER BY lineNumbers.startLine ASC;"
    };
    static char *query2 = {
        "SELECT Linenumbers.id, structId, symbolId, typeId, flags FROM LineNumbers"
        "    JOIN FileNames ON linenumbers.mainId = fileNames.id"
        "    JOIN Names ON LineNumbers.symbolId = names.id"
        "    WHERE Names.Name=?"
        "        AND fileNames.name = ? AND flags=1"
        "        AND LineNumbers.mainId != LineNumbers.fileId AND endLine = 0"
        "    ORDER BY lineNumbers.startLine ASC;"
    };
    int i, l = strlen(file);
    int rc = SQLITE_OK;
    static sqlite3_stmt *handle;
    for (i=0; i < l; i++)
        file[i] = tolower(file[i]);
    rc = prepare(db, query, strlen(query)+1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_text(handle, 1, name, strlen(name), SQLITE_STATIC);
        sqlite3_bind_text(handle, 2, file, strlen(file), SQLITE_STATIC);
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
                    *line_id = sqlite3_column_int64(handle, 0);
                    *struct_id = sqlite3_column_int64(handle, 1);
                    *sym_id = sqlite3_column_int64(handle, 2);
                    *type_id = sqlite3_column_int64(handle, 3);
                    *flags = sqlite3_column_int(handle, 4);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        finalize(handle);
        if (rc != SQLITE_OK)
        {
            sqlite3_stmt *handle2;
            rc = prepare(db, query2, strlen(query2)+1, &handle2, NULL);
            if (rc == SQLITE_OK)
            {
                int done = FALSE;
                rc = SQLITE_DONE;
                sqlite3_reset(handle2);
                sqlite3_bind_text(handle2, 1, name, strlen(name), SQLITE_STATIC);
                sqlite3_bind_text(handle2, 2, file, strlen(file), SQLITE_STATIC);
                while (!done)
                {
                    switch(rc = sqlite3_step(handle2))
                    {
                        case SQLITE_BUSY:
                            done = TRUE;
                            break;
                        case SQLITE_DONE:
                            done = TRUE;
                            break;
                        case SQLITE_ROW:
                            *line_id = sqlite3_column_int64(handle2, 0);
                            *struct_id = sqlite3_column_int64(handle2, 1);
                            *sym_id = sqlite3_column_int64(handle2, 2);
                            *type_id = sqlite3_column_int64(handle2, 3);
                            *flags = sqlite3_column_int(handle2, 4);
                            rc = SQLITE_OK;
                            done = TRUE;
                            break;
                        default:
                            done = TRUE;
                            break;
                    }
                }
                finalize(handle2);
            }
        }
    }
    return rc == SQLITE_OK;
}
char ** ccGetMatchingNames(char *name, char **names, int *size, int *max)
{
    static char *query1 = "SELECT name FROM Names WHERE name like ?;";
    static char *query2 = "SELECT name FROM StructNames WHERE name like = ?;";
    char buf[4096],*q;
    char found[256];
    int rc = SQLITE_OK;
    sqlite3_stmt *handle;
    int len = strlen(name);
    if (vacuuming)
        return names;
    strcpy(buf, name);
    strcat(buf, "%");
    rc = prepare(db, query1, strlen(query1)+1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_text(handle, 1, buf, strlen(buf), SQLITE_STATIC);
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
                case SQLITE_ROW:
                    strncpy(found, (char *)sqlite3_column_text(handle, 0), 250);
                    found[250] = 0;
                    if (!strncmp(name, found, len))
                    {
                        if (*size >= *max)
                        {
                            if (!*max) *max = 10; else *max *= 2;
                            names = realloc(names, *max * sizeof(void *));
                        }
                        names[(*size)++] = strdup(found);
                    }
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        finalize(handle);
    }
    if (rc == SQLITE_OK)
    {
        rc = prepare(db, query2, strlen(query2)+1, &handle, NULL);
        if (rc == SQLITE_OK)
        {
            int done = FALSE;
            rc = SQLITE_DONE;
            sqlite3_reset(handle);
            sqlite3_bind_text(handle, 1, buf, strlen(buf), SQLITE_STATIC);
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
                        strncpy(found, (char *) sqlite3_column_text(handle, 0), 250);
                        found[250] = 0;
                        if (!strncmp(name, found, strlen(name)))
                        {
                            if (*size >= *max)
                            {
                                if (!*max) *max = 10; else *max *= 2;
                                names = realloc(names, *max * sizeof(void *));
                            }
                            names[(*size)++] = strdup(found);
                        }
                        break;
                    default:
                        done = TRUE;
                        break;
                }
            }
            finalize(handle);
        }
    }    
    return names;
}
static int LookupName(sqlite_int64 name, char *buf)
{
    static char *query = "SELECT name FROM NAMES WHERE id = ?";
    int rc = SQLITE_OK;
    sqlite3_stmt *handle;
    rc = prepare(db, query, strlen(query)+1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_int64(handle, 1, name);
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
                    strncpy(buf, (char *)sqlite3_column_text(handle, 0), 250);
                    buf[250] = 0;
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        finalize(handle);
    }
    return rc == SQLITE_OK;
}
int ccLookupType(char *buffer, char *name, char *module, int line, int *rflags)
{
    sqlite3_int64 line_id, struct_id, sym_id, type_id;
    int flags, indirectCount;
    if (vacuuming)
        return FALSE;
    if (findLine(module, line, name, &line_id, &struct_id,
                    &sym_id, &type_id, &flags, &indirectCount))
    {
        *rflags = flags;
        return LookupName(type_id, buffer);
    }
    return FALSE;
}
int ccLookupStructType(char *name, char *module, int line, sqlite3_int64 *structId, int *indir)
{
    sqlite3_int64 line_id, struct_id, sym_id, type_id;
    int flags, indirectCount;
    if (vacuuming)
        return FALSE;
    if (findLine(module, line, name, &line_id, &struct_id,
                    &sym_id, &type_id, &flags, &indirectCount))
    {
        *structId = struct_id;
        *indir = indirectCount;
        return TRUE;
    }
    return FALSE;
}
CCSTRUCTDATA *ccLookupStructElems(char *module, sqlite3_int64 structId, int indirectCount)
{
    CCSTRUCTDATA *rv = calloc(1, sizeof(CCSTRUCTDATA));
    int rc = SQLITE_ERROR;
    int i;
    char file[MAX_PATH];
    int l = strlen(module);
    strcpy(file, module);
    for (i=0; i < l; i++)
        file[i] = tolower(file[i]);
    if (rv)
    {
        int max = 0;
        int count = 0;
        CCSTRUCTDATA *data;
        static char *query = {
           "SELECT a.name, b.name, StructFields.structId, StructFields.indirectCount FROM MemberNames a, Names b"
           "    JOIN StructFields ON StructFields.nameId = ?"
           "    JOIN FileNames on StructFields.mainId = FileNames.id OR StructFields.fileId=FileNames.id"
           "    WHERE a.id = StructFields.symbolId"
           "        AND b.id = StructFields.typeId"
           "        AND FileNames.name = ?"
           "    ORDER BY StructFields.fieldOrder ASC"
           
        } ;
        sqlite3_stmt *handle;
        rv->indirectCount = indirectCount;
        rc = prepare(db, query, strlen(query)+1, &handle, NULL);
        if (rc == SQLITE_OK)
        {
            int done = FALSE;
            rc = SQLITE_DONE;
            sqlite3_reset(handle);
            sqlite3_bind_int64(handle, 1, structId);
            sqlite3_bind_text(handle, 2, file, strlen(file), SQLITE_STATIC);
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
                    case SQLITE_ROW:
                        if (count+1 > max)
                        {
                            int old = max;
                            if (max == 0)
                            {
                                max = 20;
                            }
                            else 
                            {
                                max = max * 2;
                            }
                            rv->data = realloc(rv->data, max * sizeof(struct _structData));
                            memset(rv->data + old, 0, (max - old) * sizeof(struct _structData));
                        }
                        {
                            char *sym = (char *)sqlite3_column_text(handle, 0);
                            char *type = (char *)sqlite3_column_text(handle, 1);
                            sqlite3_int64 id = sqlite3_column_int64(handle, 2);
                            int indir = sqlite3_column_int(handle, 3);
                            rv->data[count].fieldName = strdup(sym);
                            rv->data[count].fieldType = strdup(type);
                            rv->data[count].indirectCount = indir;
                            rv->data[count].subStructId = id;
                            rv->fieldCount++, count++;
                        }
                        break;
                    default:
                        done = TRUE;
                        break;
                }
            }
            finalize(handle);
        }
    }
    if (rc != SQLITE_OK || !rv || rv->fieldCount == 0)
    {
        free(rv);
        rv = NULL;
    }
    return rv;
}
void ccFreeStructData(CCSTRUCTDATA *data)
{
    if (data)
    {
        int i;
        for (i = 0; i < data->fieldCount; i++)
        {
            free(data->data[i].fieldName);
            free(data->data[i].fieldType);
        }
        free (data->data);
        free (data);
    }
}
int ccLookupFunctionType(char *name, char *module, sqlite3_int64 *protoId, sqlite3_int64 *typeId)
{
    sqlite3_int64 line_id, struct_id, sym_id, type_id;
    int flags;
    if (vacuuming)
        return FALSE;
    if (findLine2(module, name, &line_id, &struct_id,
                    &sym_id, &type_id, &flags))
    {
        *typeId = type_id;
        *protoId = line_id;
        return !!(flags & 1);
    }
    return FALSE;
}
CCPROTODATA *ccLookupArgList(sqlite3_int64 protoId, sqlite3_int64 typeId)
{
    int rc = SQLITE_ERROR;
    CCPROTODATA *rv = calloc(1, sizeof(CCPROTODATA));
    if (rv)
    {
        static char *query = {
           "SELECT a.name, b.name FROM Names a, Names b"
           "    JOIN GlobalArgs ON GlobalArgs.lineid = ?"
           "    WHERE a.id = GlobalArgs.symbolId"
           "        AND b.id = GlobalArgs.typeId"
           "    ORDER BY GlobalArgs.fieldOrder ASC"
        } ;
        static char *query2 = {
            " SELECT name FROM Names where id = ?;"
        } ;
        sqlite3_stmt *handle;
        rc = prepare(db, query, strlen(query)+1, &handle, NULL);
        if (rc == SQLITE_OK)
        {
            int max = 0, count = 0;
            int done = FALSE;
            rc = SQLITE_DONE;
            sqlite3_reset(handle);
            sqlite3_bind_int64(handle, 1, protoId);
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
                    case SQLITE_ROW:
                        if (count+1 > max)
                        {
                            int old = max;
                            if (max == 0)
                            {
                                max = 20;
                            }
                            else 
                            {
                                max = max * 2;
                            }
                            rv->data = realloc(rv->data, max * sizeof(struct _structData));
                            memset(rv->data + old, 0, (max - old) * sizeof(struct _structData));
                        }

                        {
                            char *sym = (char *)sqlite3_column_text(handle, 0);
                            char *type = (char *)sqlite3_column_text(handle, 1);
                            rv->data[count].fieldName= strdup(sym);
                            rv->data[count].fieldType= strdup(type);
                            rv->argCount++, count++;
                        }
                        break;
                    default:
                        done = TRUE;
                        break;
                }
            }
            finalize(handle);
        }
        if (rc == SQLITE_OK && rv->argCount)
        {
            rc = prepare(db, query2, strlen(query2)+1, &handle, NULL);
            if (rc == SQLITE_OK)
            {
                int done = FALSE;
                rc = SQLITE_DONE;
                sqlite3_reset(handle);
                sqlite3_bind_int64(handle, 1, typeId);
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
                            rv->baseType = strdup((char *)sqlite3_column_text(handle, 0));
                            rc = SQLITE_OK;
                            done = TRUE;
                            break;
                        default:
                            done = TRUE;
                            break;
                    }
                }
                finalize(handle);
            }
        }
    }
    if (!rv || rc != SQLITE_OK || !rv->argCount)
    {
        free(rv);
        rv = NULL;
    }
    return rv;
}
void ccFreeArgList(CCPROTODATA *data)
{
    int i;
    if (data)
    {
        free(data->baseType);
        for (i=0; i < data->argCount; i++)
        {
            free(data->data[i].fieldName);
            free(data->data[i].fieldType);
        }
        free (data->data);
        free (data);
    }
}
BYTE * ccGetLineData(char *file, int *max)
{
    BYTE *rv = NULL;
    char buf[MAX_PATH], *p = buf;
    static char *query = {
       "SELECT lines, data FROM LineData"
       "    JOIN FileNames ON FileNames.id = LineData.fileId"
       "    WHERE fileNames.name = ?;"
    } ;
    sqlite3_stmt *handle;
    int rc = prepare(db, query, strlen(query)+1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        while (*file)
            *p++ = tolower(*file++);
        *p = 0;
        sqlite3_bind_text(handle, 1, buf, strlen(buf), SQLITE_STATIC);
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
                {
                    int n = sqlite3_column_bytes(handle, 1);
                    rv = malloc(n);
                    if (rv)
                    {
                        memcpy(rv, sqlite3_column_blob(handle, 1), n);
                        *max = sqlite3_column_int(handle, 0);
                    }   
                    done = TRUE;
                    rc = SQLITE_OK;
                }
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        finalize(handle);
    }
    return rv;
}
int ccGetColorizeData(char *file, COLORIZE_HASH_ENTRY *entries[])
{
    BYTE *rv = NULL;
    char buf[MAX_PATH], *p = buf;
    static char *query = {
       "SELECT Names.Name, FileNames.id, fileid, startline, endline, type From SymbolTypes"
       " JOIN Names on names.id = SymbolTypes.symbolId"
       " JOIN FileNames on SymbolTypes.mainId = FileNames.id OR SymbolTypes.fileId=FileNames.id"
       " WHERE FileNames.name = ?"
    } ;
    sqlite3_stmt *handle;
    int rc = prepare(db, query, strlen(query)+1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        while (*file)
            *p++ = tolower(*file++);
        *p = 0;
        sqlite3_bind_text(handle, 1, buf, strlen(buf), SQLITE_STATIC);
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
                {
                    char name[256];
                    int mainid, fileid, start, end, type;
                    strncpy(name, (char *)sqlite3_column_text(handle, 0), 250);
                    name[250] = 0;
                    mainid = sqlite3_column_int64(handle, 1);
                    fileid = sqlite3_column_int64(handle, 2);
                    start = sqlite3_column_int64(handle, 3);
                    end = sqlite3_column_int64(handle, 4);
                    type = sqlite3_column_int64(handle, 5);
                    InsertColorizeEntry(entries, name, fileid == mainid ? start : 1, fileid == mainid ? end : 0, type);
                    rc = SQLITE_OK;
                }
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        finalize(handle);
    }
    return rv;
}

static unsigned int __stdcall Start(void *);
static BOOL started;

void StartCodeCompletionServer()
{
    if (!started)
    {
        HANDLE serverThread;
        __int64 n;
        srand(time(0));
        n = rand() * (__int64)GetCurrentProcessId();
        sprintf(pipeName, "%lld", n);
        started = TRUE;
        serverThread = (HANDLE)_beginthreadex(NULL, 0, Start, NULL, 0, NULL);
    }
}
static char *GetFileData(char *filname, int *size)
{
    DWINFO *ptr;
    EDITDATA *e= NULL;
    char *rv = 0;
    WaitForSingleObject(ewSem, INFINITE);
    *size = 0;
    ptr = editWindows;
    while (ptr)
    {
        if (ptr->active && !xstricmp(filname,ptr->dwName))
        {
            e = ptr->editData;
            break;
        }
        ptr = ptr->next;
    }
    if (!e || !e->cd->textlen || !e->cd->text)
    {
        FILE *fil;
        SetEvent(ewSem);
        fil = fopen(filname,"rb");
        if (fil)
        {
            int n;
            fseek(fil, 0, SEEK_END);
            n = ftell(fil);
            if (n < 1024 * 1024)
            {
                char *buf = malloc(n+1);
                fseek(fil,0, SEEK_SET);
                if (buf)
                {
                    fread(buf, 1, n, fil);
                    rv = buf;
                    *size = n;
                    buf[n] = 0;
                }
            }
            fclose(fil);
        }
    }
    else
    {
        int n = e->cd->textlen;
        rv = malloc(n + 1);
        if (rv)
        {
            char *p = rv;
            int i;
            INTERNAL_CHAR *s = e->cd->text;
            rv[n] = 0;
            for (i=0; i < n; i++)
                *p++ = s->ch, s++;
            *size = n;
        }
        SetEvent(ewSem);
    }
    return rv;
}
static DWORD serverProc(void *b)
{
    HANDLE myPipe = (HANDLE )b;
    int n;
    DWORD read;
    if (ReadFile(myPipe, &n, sizeof(DWORD), &read, NULL))
    {
        char filname[256];
        if (ReadFile(myPipe, filname, n, &read , NULL))
        {
            char *buf ;
            filname[n] = 0 ;
            buf = GetFileData(filname, &n);
            WriteFile(myPipe, &n, sizeof(DWORD), &read, NULL);
            if (n)
                WriteFile(myPipe, buf, n, &read, NULL);
        }
    }
    FlushFileBuffers(myPipe);
    DisconnectNamedPipe(myPipe);
    CloseHandle(myPipe);
    return 0;
}
static unsigned int __stdcall Start(void *aa)
{
    char pipe[260];
    HANDLE serverPipe = 0;
    sprintf(pipe,"\\\\.\\pipe\\%s", pipeName);
                                 
                                                              
    while (serverPipe != INVALID_HANDLE_VALUE)
    {
        DWORD n;
        DWORD read;
        serverPipe = CreateNamedPipe(pipe, PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | 8/* to reject remote connections */,
                                     PIPE_UNLIMITED_INSTANCES,100000,10000,0,NULL);
        if (serverPipe != INVALID_HANDLE_VALUE)
        {

            BOOL fConnected = ConnectNamedPipe(serverPipe, NULL) ? 
                    TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 
              if (fConnected) 
              { 
                 _beginthread((BEGINTHREAD_FUNC)serverProc, 0, (LPVOID)serverPipe);
               } 
              else 
                 CloseHandle(serverPipe); 
        }
    }
    return 0;
}
