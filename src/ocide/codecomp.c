/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <richedit.h>
#include "header.h"
#include <sys/stat.h>
#include <process.h>
#include <time.h>
#include <ctype.h>
#include "symtypes.h"
#include <limits.h>

#define FROMIDE

#define DBVersion 121

extern PROJECTITEM* workArea;
extern DWINFO* editWindows;
extern char szInstallPath[];
extern HANDLE editHeap;
extern HANDLE ewSem;
extern HANDLE makeSem;
extern int defaultWorkArea;

static char prm_searchpath[10000];
static char sys_searchpath[256];

static char pipeName[MAX_PATH];

static char* localEmptyString = "";
static sqlite3* db;

static int version_ok;

static HANDLE ccEvent;

static BOOL vacuuming;

void DBClose(sqlite3* db);
char* getcwd(char* __buf, int __buflen);  // can't include dir.h because it defines eof...

void ReloadLineData(char* name)
{
    DWINFO* ptr = editWindows;
    DWINFO info;
    strcpy(info.dwName, name);
    while (ptr)
    {
        if (ptr->active && SendMessage(ptr->self, WM_COMMAND, ID_QUERYHASFILE, (LPARAM)&info))
        {
            PostMessage(ptr->dwHandle, EM_LOADLINEDATA, 0, 0);
            InvalidateRect(ptr->dwHandle, 0, 0);
            InvalidateRect(ptr->self, 0, 0);
            break;
        }
        ptr = ptr->next;
    }
}
static int prepare(sqlite3* db, char* name, int len, sqlite3_stmt** handle, char** vv)
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
static int finalize(sqlite3_stmt* handle)
{
    int rv = sqlite3_finalize(handle);
    SetEvent(ccEvent);
    return rv;
}

static int verscallback(void* NotUsed, int argc, char** argv, char** azColName)
{
    if (argc == 1)
    {
        if (atoi(argv[0]) >= DBVersion)
            version_ok = TRUE;
    }
    return 0;
}
static int busyFunc(void* handle, int times)
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
    else
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            ProcessMessage(&msg);
            if (msg.message == WM_QUIT)
                break;
        }
        Sleep(0);
        return 1;
    }
}
static void dbVacuum(void* xx)
{
    char* zErrMsg = NULL;
    sqlite3* xdb = xx;
    DWINFO* p;
    if (sqlite3_exec(xdb, "PRAGMA journal_mode = OFF; PRAGMA synchronous = OFF;PRAGMA temp_store = MEMORY;",  // VACUUM;",
                     verscallback, 0, &zErrMsg) != SQLITE_OK)
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
void ccDBVacuum(sqlite3* db)
{
    if (db)
    {
        vacuuming = TRUE;
        _beginthread((BEGINTHREAD_FUNC)dbVacuum, 0, (LPVOID)db);
    }
}
void ccDBSetDB(sqlite3* xdb) { db = xdb; }
sqlite3* ccDBOpen(PROJECTITEM* pj)
{
    sqlite3* xdb;
    char* zErrMsg = NULL;
    char name[MAX_PATH], *p;
    strcpy(name, pj->realName);
    p = strrchr(name, '.');
    if (!p)
        p = name + strlen(name);
    strcpy(p, ".ods");
    DBClose(db);
    db = NULL;
    vacuuming = FALSE;
    if (sqlite3_open_v2(name, &xdb, SQLITE_OPEN_READWRITE, NULL) == SQLITE_OK)
    {
        char* zErrMsg = NULL;
        version_ok = FALSE;
        if (sqlite3_exec(xdb, "SELECT value FROM ccPropertyBag WHERE property = \"ccVersion\"", verscallback, 0, &zErrMsg) !=
                SQLITE_OK ||
            !version_ok)
        {
            sqlite3_free(zErrMsg);
            DBClose(xdb);
            xdb = NULL;
            if (!version_ok)
                unlink(name);
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
static void DoCompleteCommand(PROJECTITEM* pj)
{
    char* cmd = Lookup("__CODE_COMPLETION", pj, NULL);
    if (cmd)
    {
        PROJECTITEM* project = pj;
        while (project && project->type != PJ_PROJ)
            project = project->parent;
        Execute(cmd, project->realName, ERR_NO_WINDOW);
        free(cmd);
    }
}
static int FindCompleteCommand(PROJECTITEM* pj, char* name)
{
    int rv = 0;
    do
    {
        if (pj->type == PJ_FILE)
        {
            BOOL smattered = FALSE;
            char buf[10];
            char* p;
            if (!xstricmp(pj->realName + strlen(pj->realName) - 2, ".h") ||
                !xstricmp(pj->realName + strlen(pj->realName) - 4, ".hpp") ||
                !xstricmp(pj->realName + strlen(pj->realName) - 4, ".hxx"))
            {
                // all this is to look up the rules for a .h file as if it were a .c
                // file, we do it this way rather than goofing with the rule file
                // because otherwise we would start compiling .h files..
                p = strrchr(pj->realName, '.');
                strcpy(buf, p);
                strcpy(p, ".c");

                smattered = TRUE;
            }
            AddSymbolTable(pj, FALSE);
            if (smattered)
            {
                strcpy(p, buf);
            }
            if (!xstricmp(name, pj->realName))
            {
                DoCompleteCommand(pj);
                RemoveSymbolTable();
                break;
            }
        }
        else if (pj->children)
        {
            AddSymbolTable(pj, FALSE);

            FindCompleteCommand(pj->children, name);
        }
        RemoveSymbolTable();
        pj = pj->next;
    } while (pj);
    return rv;
}
static BOOL changed(char* name)
{
    char lcname[MAX_PATH], *p = lcname, *q = name;
    struct stat statbuf;
    static char* query = {
        "SELECT fileDate FROM FileNames"
        "    WHERE fileNames.name = ?;"};
    BOOL idefile;
    sqlite3_stmt* handle;
    int rc;
    time_t t;
    DWINFO* ptr;
    EDITDATA* e;
    if (vacuuming)
        return FALSE;
    WaitForSingleObject(ewSem, INFINITE);
    ptr = editWindows;
    while (ptr)
    {
        if (ptr->active && !xstricmp(name, ptr->dwName))
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
    while (*q)
        *p++ = tolower(*q++);
    *p = 0;
    rc = prepare(db, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_text(handle, 1, lcname, strlen(lcname), SQLITE_STATIC);
        while (!done)
        {
            switch (rc = sqlite3_step(handle))
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
void DoParse(char* name)
{
    if (vacuuming)
        return;
    if (workArea && changed(name))
    {
        char dbName[MAX_PATH], *p;
        char buf[256];
        PROJECTITEM* pj = workArea;
        sprintf(buf, "Calculating completion data for %s", name);
        SetStatusMessage(buf, FALSE);
        strcpy(dbName, pj->realName);
        p = strrchr(dbName, '.');
        if (!p)
            p = dbName + strlen(dbName);
        strcpy(p, ".ods");
        if (defaultWorkArea)  // this is a convenience so I an use code completion without a project
        {
            char cmd[10000];
            char cwd[MAX_PATH];
            getcwd(cwd, MAX_PATH);
            strcat(cwd, "\\xx");
            sprintf(cmd, "\"%s\\bin\\occpr.exe\" /1 \"-o%s\" /P%s \"%s\"", szInstallPath, dbName, pipeName, name);
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
        if (workArea == pj && db == NULL && !vacuuming)
            ccDBSetDB(ccDBOpen(pj));
        ReloadLineData(name);
    }
}
void deleteFileData(char* name) {}
int ccLookupUsing(char* file, int lineno, int parent_id, char* ns)
{
    static char* query = {
        "SELECT names.name FROM names"
        "    JOIN UsingData on Names.id = usingData.symbolId"
        "    Join FileNames ON UsingData.fileId = fileNames.id"
        "    WHERE fileNames.name = ?"
        "    AND usingData.parentId = ?"
        "    AND ? >= usingData.startLine;"};
    int startline = 0, endline = INT_MAX;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = prepare(db, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_text(handle, 1, file, strlen(file), SQLITE_STATIC);
        sqlite3_bind_int(handle, 2, parent_id);
        sqlite3_bind_int(handle, 3, lineno);
        while (!done)
        {
            switch (rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    done = TRUE;
                    break;
                case SQLITE_ROW:
                {
                    char* text = sqlite3_column_text(handle, 0);
                    if (ns[0])
                        strcat(ns, ";");
                    strcat(ns, text);
                    rc = SQLITE_OK;
                    break;
                }
                default:
                    done = TRUE;
                    break;
            }
        }
        finalize(handle);
    }
    return rc;
}
int ccLookupContainingNamespace(char* file, int lineno, char* ns)
{
    static char* query = {
        "SELECT names.name, NameSpaceData.startLine, NameSpaceData.endLine, names.id FROM Names"
        "    JOIN NameSpaceData ON Names.id = NameSpaceData.symbolId"
        "    JOIN FileNames ON NameSpaceData.fileId = fileNames.id"
        "    WHERE fileNames.name = ?"
        "        AND ? >= NameSpaceData.startLine AND ? <= NameSpaceData.endLine;"};
    int startline = 0, endline = INT_MAX;
    int parent_id = -1;
    int i, l = strlen(file);
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    ns[0] = 0;
    for (i = 0; i < l; i++)
        file[i] = tolower(file[i]);
    rc = prepare(db, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_text(handle, 1, file, strlen(file), SQLITE_STATIC);
        sqlite3_bind_int(handle, 2, lineno);
        sqlite3_bind_int(handle, 3, lineno);
        while (!done)
        {
            switch (rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    done = TRUE;
                    break;
                case SQLITE_ROW:
                {
                    int cstart = sqlite3_column_int(handle, 1);
                    int cend = sqlite3_column_int(handle, 2);
                    if (cstart > startline && cend < endline)
                    {
                        startline = cstart;
                        endline = cend;
                        strcpy(ns, sqlite3_column_text(handle, 0));
                        parent_id = sqlite3_column_int(handle, 3);
                    }
                    rc = SQLITE_OK;
                    break;
                }
                default:
                    done = TRUE;
                    break;
            }
        }
        finalize(handle);
    }
    ccLookupUsing(file, lineno, parent_id, ns);
    return rc;
}
// this really returns the stem not the entire function name...
int ccLookupContainingMemberFunction(char* file, int lineno, char* func)
{
    static char* query = {
        "SELECT names.name FROM Names"
        "    JOIN LineNumbers ON Names.id = LineNumbers.symbolId"
        "    JOIN FileNames ON linenumbers.fileId = fileNames.id"
        "    WHERE fileNames.name = ?  AND LineNumbers.altendLine != 0 "  // altline is currently only used for
        "        AND ? >= LineNumbers.startLine AND ? <= LineNumbers.altendLine;"};
    int i, l = strlen(file);
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    func[0] = 0;
    for (i = 0; i < l; i++)
        file[i] = tolower(file[i]);
    rc = prepare(db, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_text(handle, 1, file, strlen(file), SQLITE_STATIC);
        sqlite3_bind_int(handle, 2, lineno);
        sqlite3_bind_int(handle, 3, lineno);
        while (!done)
        {
            switch (rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    done = TRUE;
                    break;
                case SQLITE_ROW:
                    strcpy(func, sqlite3_column_text(handle, 0));
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
    if (func[0])
    {
        char* p = func;
        int nesting = 0;
        char* last = p;
        while (*p)
        {
            if (*p == '@' && !nesting)
                last = p;
            else if (*p == '#')
                nesting++;
            else if (*p == '~' && nesting)
                nesting--;
            p++;
        }
        *last = '\0';
        rc = TRUE;
    }
    else
    {
        rc = FALSE;
    }

    return rc;
}
static int findLine(char* file, int line, char* name, sqlite3_int64* line_id, sqlite3_int64* struct_id, sqlite_int64* sym_id,
                    sqlite_int64* type_id, int* flags, int* indirectCount)
{
    // this query to attempt to find it in the current file
    static char* query = {
        "SELECT Linenumbers.id, structId, symbolId, typeId, flags, indirectCount FROM LineNumbers"
        "    JOIN Names ON LineNumbers.symbolId = names.id"
        "    JOIN FileNames ON linenumbers.fileId = fileNames.id"
        "    WHERE Names.Name=?"
        "        AND fileNames.name = ?"
        "        AND ? >= startLine AND (? <= endLine OR endLine = 0);"
        //        "    ORDER BY lineNumbers.startLine ASC;"
    };
    static char* query2 = {
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
    static sqlite3_stmt* handle;
    for (i = 0; i < l; i++)
        file[i] = tolower(file[i]);
    rc = prepare(db, query, strlen(query) + 1, &handle, NULL);
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
            switch (rc = sqlite3_step(handle))
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
            sqlite3_stmt* handle2;
            rc = prepare(db, query2, strlen(query2) + 1, &handle2, NULL);
            if (rc == SQLITE_OK)
            {
                int done = FALSE;
                rc = SQLITE_DONE;
                sqlite3_reset(handle2);
                sqlite3_bind_text(handle2, 1, name, strlen(name), SQLITE_STATIC);
                sqlite3_bind_text(handle2, 2, file, strlen(file), SQLITE_STATIC);
                while (!done)
                {
                    switch (rc = sqlite3_step(handle2))
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
static int findLine2(char* file, char* name, sqlite3_int64* line_id, sqlite3_int64* struct_id, sqlite_int64* sym_id,
                     sqlite_int64* type_id, int* flags)
{
    // this query to attempt to find it in the current file
    static char* query = {
        "SELECT Linenumbers.id, structId, symbolId, typeId, flags FROM LineNumbers"
        "    JOIN FileNames ON linenumbers.fileId = fileNames.id"
        "    JOIN Names ON LineNumbers.symbolId = names.id"
        "    WHERE Names.Name=?"
        "        AND fileNames.name = ? AND endLine = 0 AND flags=1"
        "    ORDER BY lineNumbers.startLine ASC;"};
    static char* query2 = {
        "SELECT Linenumbers.id, structId, symbolId, typeId, flags FROM LineNumbers"
        "    JOIN FileNames ON linenumbers.mainId = fileNames.id"
        "    JOIN Names ON LineNumbers.symbolId = names.id"
        "    WHERE Names.Name=?"
        "        AND fileNames.name = ? AND flags=1"
        "        AND LineNumbers.mainId != LineNumbers.fileId AND endLine = 0"
        "    ORDER BY lineNumbers.startLine ASC;"};
    int i, l = strlen(file);
    int rc = SQLITE_OK;
    static sqlite3_stmt* handle;
    for (i = 0; i < l; i++)
        file[i] = tolower(file[i]);
    rc = prepare(db, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_text(handle, 1, name, strlen(name), SQLITE_STATIC);
        sqlite3_bind_text(handle, 2, file, strlen(file), SQLITE_STATIC);
        while (!done)
        {
            switch (rc = sqlite3_step(handle))
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
            sqlite3_stmt* handle2;
            rc = prepare(db, query2, strlen(query2) + 1, &handle2, NULL);
            if (rc == SQLITE_OK)
            {
                int done = FALSE;
                rc = SQLITE_DONE;
                sqlite3_reset(handle2);
                sqlite3_bind_text(handle2, 1, name, strlen(name), SQLITE_STATIC);
                sqlite3_bind_text(handle2, 2, file, strlen(file), SQLITE_STATIC);
                while (!done)
                {
                    switch (rc = sqlite3_step(handle2))
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
char** ccGetMatchingNames(char* name, char** names, int* size, int* max)
{
    static char* query1 = "SELECT name FROM Names WHERE name like ?;";
    static char* query2 = "SELECT name FROM StructNames WHERE name like = ?;";
    char buf[4096];
    char found[256];
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    int len = strlen(name);
    if (vacuuming)
        return names;
    strcpy(buf, name);
    strcat(buf, "%");
    rc = prepare(db, query1, strlen(query1) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_text(handle, 1, buf, strlen(buf), SQLITE_STATIC);
        while (!done)
        {
            switch (rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    done = TRUE;
                    rc = SQLITE_OK;
                    break;
                case SQLITE_ROW:
                    strncpy(found, (char*)sqlite3_column_text(handle, 0), 250);
                    found[250] = 0;
                    if (!strncmp(name, found, len))
                    {
                        if (*size >= *max)
                        {
                            if (!*max)
                                *max = 10;
                            else
                                *max *= 2;
                            names = realloc(names, *max * sizeof(void*));
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
        rc = prepare(db, query2, strlen(query2) + 1, &handle, NULL);
        if (rc == SQLITE_OK)
        {
            int done = FALSE;
            rc = SQLITE_DONE;
            sqlite3_reset(handle);
            sqlite3_bind_text(handle, 1, buf, strlen(buf), SQLITE_STATIC);
            while (!done)
            {
                switch (rc = sqlite3_step(handle))
                {
                    case SQLITE_BUSY:
                        done = TRUE;
                        break;
                    case SQLITE_DONE:
                        done = TRUE;
                        break;
                    case SQLITE_ROW:
                        strncpy(found, (char*)sqlite3_column_text(handle, 0), 250);
                        found[250] = 0;
                        if (!strncmp(name, found, strlen(name)))
                        {
                            if (*size >= *max)
                            {
                                if (!*max)
                                    *max = 10;
                                else
                                    *max *= 2;
                                names = realloc(names, *max * sizeof(void*));
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
static int LookupName(sqlite_int64 name, char* buf)
{
    static char* query = "SELECT name FROM NAMES WHERE id = ?";
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = prepare(db, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_int64(handle, 1, name);
        while (!done)
        {
            switch (rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    done = TRUE;
                    break;
                case SQLITE_ROW:
                    strncpy(buf, (char*)sqlite3_column_text(handle, 0), 250);
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
int ccLookupType(char* buffer, char* name, char* module, int line, int* rflags, sqlite3_int64* rtype_id)
{
    sqlite3_int64 line_id, struct_id, sym_id, type_id;
    int flags, indirectCount;
    if (vacuuming)
        return FALSE;
    if (findLine(module, line, name, &line_id, &struct_id, &sym_id, &type_id, &flags, &indirectCount))
    {
        *rflags = flags;
        if (rtype_id)
            *rtype_id = type_id;
        return LookupName(type_id, buffer);
    }
    return FALSE;
}
int ccLookupStructType(char* name, char* module, int line, sqlite3_int64* structId, int* indir)
{
    sqlite3_int64 line_id, struct_id, sym_id, type_id;
    int flags, indirectCount;
    if (vacuuming)
        return FALSE;
    if (findLine(module, line, name, &line_id, &struct_id, &sym_id, &type_id, &flags, &indirectCount))
    {
        *structId = struct_id;
        *indir = indirectCount;
        return TRUE;
    }
    return FALSE;
}
int ccLookupStructId(char* name, char* module, int line, sqlite3_int64* structId)
{
    char* query = "SELECT id From StructNames where name = ?;";
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = prepare(db, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_text(handle, 1, name, strlen(name), SQLITE_STATIC);
        while (!done)
        {
            switch (rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    done = TRUE;
                    break;
                case SQLITE_ROW:
                    *structId = sqlite3_column_int(handle, 0);
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
int ccLookupMemberType(char* name, char* module, int line, sqlite3_int64* structId, int* indir)
{
    char* query =
        "SELECT structid, indirectCount From structfields"
        "  join membernames on membernames.id = structfields.symbolid"
        "    where membernames.name = ?;";
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = prepare(db, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_reset(handle);
        sqlite3_bind_text(handle, 1, name, strlen(name), SQLITE_STATIC);
        while (!done)
        {
            switch (rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    done = TRUE;
                    break;
                case SQLITE_ROW:
                    *structId = sqlite3_column_int(handle, 0);
                    *indir = sqlite3_column_int(handle, 1);
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
CCSTRUCTDATA* ccLookupStructElems(char* module, sqlite3_int64 structId, int indirectCount)
{
    CCSTRUCTDATA* rv = calloc(1, sizeof(CCSTRUCTDATA));
    int rc = SQLITE_ERROR;
    int i;
    char file[MAX_PATH];
    int l = strlen(module);
    strcpy(file, module);
    for (i = 0; i < l; i++)
        file[i] = tolower(file[i]);
    if (rv)
    {
        int max = 0;
        int count = 0;
        static char* query = {
            "SELECT a.name, b.name, StructFields.structId, StructFields.indirectCount, StructFields.flags FROM MemberNames a, "
            "Names b"
            "    JOIN StructFields ON StructFields.nameId = ?"
            "    JOIN FileNames on StructFields.mainId = FileNames.id OR StructFields.fileId=FileNames.id"
            "    WHERE a.id = StructFields.symbolId"
            "        AND b.id = StructFields.typeId"
            "        AND FileNames.name = ?"
            "    ORDER BY StructFields.fieldOrder ASC"

        };
        sqlite3_stmt* handle;
        rv->indirectCount = indirectCount;
        rc = prepare(db, query, strlen(query) + 1, &handle, NULL);
        if (rc == SQLITE_OK)
        {
            int done = FALSE;
            rc = SQLITE_DONE;
            sqlite3_reset(handle);
            sqlite3_bind_int64(handle, 1, structId);
            sqlite3_bind_text(handle, 2, file, strlen(file), SQLITE_STATIC);
            while (!done)
            {
                switch (rc = sqlite3_step(handle))
                {
                    case SQLITE_BUSY:
                        done = TRUE;
                        break;
                    case SQLITE_DONE:
                        done = TRUE;
                        rc = SQLITE_OK;
                        break;
                    case SQLITE_ROW:
                        if (count + 1 > max)
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
                            char* sym = (char*)sqlite3_column_text(handle, 0);
                            char* type = (char*)sqlite3_column_text(handle, 1);
                            sqlite3_int64 id = sqlite3_column_int64(handle, 2);
                            int indir = sqlite3_column_int(handle, 3);
                            int flags = sqlite3_column_int(handle, 4);
                            rv->data[count].fieldName = strdup(sym);
                            rv->data[count].fieldType = strdup(type);
                            rv->data[count].indirectCount = indir;
                            rv->data[count].flags = flags;
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
void ccFreeStructData(CCSTRUCTDATA* data)
{
    if (data)
    {
        int i;
        for (i = 0; i < data->fieldCount; i++)
        {
            free(data->data[i].fieldName);
            free(data->data[i].fieldType);
        }
        free(data->data);
        free(data);
    }
}
int ccLookupFunctionType(char* name, char* module, sqlite3_int64* protoId, sqlite3_int64* typeId)
{
    sqlite3_int64 line_id, struct_id, sym_id, type_id;
    int flags;
    if (vacuuming)
        return FALSE;
    if (findLine2(module, name, &line_id, &struct_id, &sym_id, &type_id, &flags))
    {
        *typeId = type_id;
        *protoId = line_id;
        return !!(flags & 1);
    }
    return FALSE;
}
CCPROTODATA* ccLookupArgList(sqlite3_int64 protoId, sqlite3_int64 typeId)
{
    int rc = SQLITE_ERROR;
    CCPROTODATA* rv = calloc(1, sizeof(CCPROTODATA));
    if (rv)
    {
        static char* query = {
            "SELECT a.name, b.name FROM Names a, Names b"
            "    JOIN GlobalArgs ON GlobalArgs.lineid = ?"
            "    WHERE a.id = GlobalArgs.symbolId"
            "        AND b.id = GlobalArgs.typeId"
            "    ORDER BY GlobalArgs.fieldOrder ASC"};
        static char* query2 = {" SELECT name FROM Names where id = ?;"};
        sqlite3_stmt* handle;
        rc = prepare(db, query, strlen(query) + 1, &handle, NULL);
        if (rc == SQLITE_OK)
        {
            int max = 0, count = 0;
            int done = FALSE;
            rc = SQLITE_DONE;
            sqlite3_reset(handle);
            sqlite3_bind_int64(handle, 1, protoId);
            while (!done)
            {
                switch (rc = sqlite3_step(handle))
                {
                    case SQLITE_BUSY:
                        done = TRUE;
                        break;
                    case SQLITE_DONE:
                        done = TRUE;
                        rc = SQLITE_OK;
                        break;
                    case SQLITE_ROW:

                    {
                        char* sym = (char*)sqlite3_column_text(handle, 0);
                        char* type = (char*)sqlite3_column_text(handle, 1);
                        if (count != 0 || strcmp(sym, "_this"))
                        {
                            if (count + 1 > max)
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
                            rv->data[count].fieldName = strdup(sym);
                            rv->data[count].fieldType = strdup(type);
                            rv->argCount++, count++;
                        }
                        else
                        {
                            rv->member = TRUE;
                        }
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
            rc = prepare(db, query2, strlen(query2) + 1, &handle, NULL);
            if (rc == SQLITE_OK)
            {
                int done = FALSE;
                rc = SQLITE_DONE;
                sqlite3_reset(handle);
                sqlite3_bind_int64(handle, 1, typeId);
                while (!done)
                {
                    switch (rc = sqlite3_step(handle))
                    {
                        case SQLITE_BUSY:
                            done = TRUE;
                            break;
                        case SQLITE_DONE:
                            done = TRUE;
                            break;
                        case SQLITE_ROW:
                            rv->baseType = strdup((char*)sqlite3_column_text(handle, 0));
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
void ccFreeArgList(CCPROTODATA* data)
{
    int i;
    if (data)
    {
        free(data->baseType);
        for (i = 0; i < data->argCount; i++)
        {
            free(data->data[i].fieldName);
            free(data->data[i].fieldType);
        }
        free(data->data);
        free(data);
    }
}
BYTE* ccGetLineData(char* file, int* max)
{
    BYTE* rv = NULL;
    char buf[MAX_PATH], *p = buf;
    static char* query = {
        "SELECT lines, data FROM LineData"
        "    JOIN FileNames ON FileNames.id = LineData.fileId"
        "    WHERE fileNames.name = ?;"};
    sqlite3_stmt* handle;
    int rc = prepare(db, query, strlen(query) + 1, &handle, NULL);
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
            switch (rc = sqlite3_step(handle))
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
int ccGetColorizeData(char* file, COLORIZE_HASH_ENTRY* entries[])
{
    BYTE* rv = NULL;
    char buf[MAX_PATH], *p = buf;
    static char* query = {
        "SELECT Names.Name, FileNames.id, fileid, startline, endline, type From SymbolTypes"
        " JOIN Names on names.id = SymbolTypes.symbolId"
        " JOIN FileNames on SymbolTypes.mainId = FileNames.id OR SymbolTypes.fileId=FileNames.id"
        " WHERE FileNames.name = ?"};
    sqlite3_stmt* handle;
    int rc = prepare(db, query, strlen(query) + 1, &handle, NULL);
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
            switch (rc = sqlite3_step(handle))
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
                    strncpy(name, (char*)sqlite3_column_text(handle, 0), 250);
                    name[250] = 0;
                    mainid = sqlite3_column_int64(handle, 1);
                    fileid = sqlite3_column_int64(handle, 2);
                    start = sqlite3_column_int64(handle, 3);
                    end = sqlite3_column_int64(handle, 4);
                    type = sqlite3_column_int64(handle, 5);
                    InsertColorizeEntry(entries, name, fileid == mainid && type != ST_FUNCTION ? start : 1,
                                        fileid == mainid && type != ST_FUNCTION ? end : 0, type);
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
BOOL ccExistsInDB(char* file)
{
    BOOL rv = FALSE;
    char buf[MAX_PATH], *p = buf;
    static char* query = {"SELECT Count(id) From FileNames where FileNames.name = ?"};
    sqlite3_stmt* handle;
    int rc = prepare(db, query, strlen(query) + 1, &handle, NULL);
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
            switch (rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    done = TRUE;
                    break;
                case SQLITE_ROW:
                {
                    rv = sqlite3_column_int64(handle, 0) != 0;
                    rc = SQLITE_OK;
                    done = TRUE;
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
static unsigned int __stdcall Start(void*);
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
static char* GetFileData(char* filname, int* size)
{
    DWINFO* ptr;
    EDITDATA* e = NULL;
    char* rv = 0;
    WaitForSingleObject(ewSem, INFINITE);
    *size = 0;
    ptr = editWindows;
    while (ptr)
    {
        if (ptr->active && !xstricmp(filname, ptr->dwName))
        {
            e = ptr->editData;
            break;
        }
        ptr = ptr->next;
    }
    if (!e || !e->cd->textlen || !e->cd->text)
    {
        FILE* fil;
        SetEvent(ewSem);
        fil = fopen(filname, "rb");
        if (fil)
        {
            int n;
            fseek(fil, 0, SEEK_END);
            n = ftell(fil);
            if (n < 1024 * 1024)
            {
                char* buf = malloc(n + 1);
                fseek(fil, 0, SEEK_SET);
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
            char* p = rv;
            int i;
            INTERNAL_CHAR* s = e->cd->text;
            rv[n] = 0;
            for (i = 0; i < n; i++)
                *p++ = s->ch, s++;
            *size = n;
        }
        SetEvent(ewSem);
    }
    return rv;
}
struct ServerData
{
    char *data;
    HANDLE handle;
};
static DWORD servupProc(void *b)
{
    struct ServerData* data = (struct ServerData*)b;
    BOOL fConnected = ConnectNamedPipe(data->handle, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
    if (fConnected && data->data)
    {
        DWORD read, n = strlen(data->data);
        int ofs = 0;
        while (n > 0)
        {
            int len = n > 8192 ? 8192 : n;
            if (!WriteFile(data->handle, data->data+ofs, len, &read, NULL))
                break;
            n -= len;
            ofs += len;
        }
    }
    FlushFileBuffers(data->handle);
    DisconnectNamedPipe(data->handle);
    CloseHandle(data->handle);
    free(data->data);
    free(data);
}
static unsigned int __stdcall Start(void* aa)
{
    char pipe[260];
    HANDLE serverPipe = 0;
    sprintf(pipe, "\\\\.\\pipe\\%s", pipeName);
    srand(GetTickCount());

    while (serverPipe != INVALID_HANDLE_VALUE)
    {
        serverPipe = CreateNamedPipe(pipe, PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
                                     PIPE_UNLIMITED_INSTANCES, 8192, 8192, 0, NULL);
        if (serverPipe != INVALID_HANDLE_VALUE)
        {

            BOOL fConnected = ConnectNamedPipe(serverPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
            if (fConnected)
            {
                DWORD read;
                DWORD n;
                while (ReadFile(serverPipe, &n, sizeof(DWORD), &read, NULL))
                {
                    char filname[256];
                    if (ReadFile(serverPipe, filname, n, &read, NULL))
                    {
                        char* buf;
                        filname[n] = 0;
                        char name[256];
                        sprintf(name, "%s%d-%d", pipeName, rand(), rand());
                        char pipe[MAX_PATH];
                        sprintf(pipe, "\\\\.\\pipe\\%s", name);
                        HANDLE handle;
                        handle = CreateNamedPipe(pipe, PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
                            PIPE_UNLIMITED_INSTANCES, 8192, 8192, 0, NULL);
                        if (handle != INVALID_HANDLE_VALUE)
                        {
                            struct ServerData *data = (struct ServerData *)calloc(1, sizeof(struct ServerData));
                            data->data = GetFileData(filname, &n);
                            data->handle = handle;
                            int n = strlen(name);
                            DWORD read;
                            WriteFile(serverPipe, &n, sizeof(DWORD), &read, NULL);
                            WriteFile(serverPipe, name, n, &read, NULL);
                            _beginthread(servupProc, 0, data);
                        }
                    }
                }
                DisconnectNamedPipe(serverPipe);
            }
        }
    }
    return 0;
}
CCFUNCDATA* ccLookupFunctionList(int lineno, char* file, char* name)
{
    CCFUNCDATA *funcs = NULL, **scan;
    sqlite_int64 id, baseid;
    CCPROTODATA* args;
    if (ccLookupFunctionType(name, file, &id, &baseid) && (args = ccLookupArgList(id, baseid)))
    {
        funcs = calloc(1, sizeof(CCFUNCDATA));
        funcs->fullname = strdup(name);
        funcs->args = args;
    }
    else
    {
        int ids[1000];
        int count = 0;
        static char* query = {"SELECT id from NAMES where name = ?;"};
        static char* query1 = {
            " SELECT Names.name FROM LineNumbers "
            "    JOIN names on  names.id = linenumbers.symbolid"
            "    Join FileNames on fileNames.id = linenumbers.fileid"
            "    JOIN cppNameMapping ON cppnamemapping.complexId = linenumbers.symbolid "
            "           and cppnamemapping.mainid = filenames.id"
            "    WHERE CPPNameMapping.simpleId = ? AND FileNames.name= ? and "
            "         LineNumbers.startLine < ? and (linenumbers.endline == 0 OR lineNumbers.endline > ?);"};
        static char* query2 = {
            " SELECT Names.name FROM LineNumbers "
            "    JOIN names on  names.id = linenumbers.symbolid"
            "    Join FileNames on fileNames.id = linenumbers.mainid"
            "    JOIN cppNameMapping ON cppnamemapping.complexId = linenumbers.symbolid "
            "           and cppnamemapping.mainid = filenames.id"
            "    WHERE CPPNameMapping.simpleId = ? AND FileNames.name= ?;"};
        sqlite_int64 id, baseid;
        int i, l = strlen(file);
        int rc = SQLITE_OK;
        sqlite3_stmt* handle;
        for (i = 0; i < l; i++)
            file[i] = tolower(file[i]);
        rc = prepare(db, query, strlen(query) + 1, &handle, NULL);
        if (rc == SQLITE_OK)
        {
            int done = FALSE;
            rc = SQLITE_DONE;
            sqlite3_reset(handle);
            sqlite3_bind_text(handle, 1, name, strlen(name), SQLITE_STATIC);
            while (!done)
            {
                switch (sqlite3_step(handle))
                {
                    case SQLITE_BUSY:
                        done = TRUE;
                        break;
                    case SQLITE_DONE:
                        done = TRUE;
                        break;
                    case SQLITE_ROW:
                    {
                        if (count < 1000)
                            ids[count++] = sqlite3_column_int(handle, 0);
                        rc = SQLITE_OK;
                        break;
                    }
                    default:
                        done = TRUE;
                        break;
                }
            }
            finalize(handle);
        }
        if (rc == SQLITE_OK)
        {
            int i;
            for (i = 0; i < count; i++)
            {
                rc = prepare(db, query1, strlen(query1) + 1, &handle, NULL);
                if (rc == SQLITE_OK)
                {
                    int done = FALSE;
                    rc = SQLITE_DONE;
                    sqlite3_reset(handle);
                    sqlite3_bind_int(handle, 1, ids[i]);
                    sqlite3_bind_text(handle, 2, file, strlen(file), SQLITE_STATIC);
                    sqlite3_bind_int64(handle, 3, lineno);
                    sqlite3_bind_int64(handle, 4, lineno);
                    while (!done)
                    {
                        switch (rc = sqlite3_step(handle))
                        {
                            case SQLITE_BUSY:
                                done = TRUE;
                                break;
                            case SQLITE_DONE:
                                done = TRUE;
                                break;
                            case SQLITE_ROW:
                            {
                                CCFUNCDATA* next = calloc(1, sizeof(CCFUNCDATA));
                                ids[count] = -1;
                                //                                scan = &funcs;
                                //                                while (*scan)
                                //                                    scan = &(*scan)->next;
                                //                                *scan = next;
                                next->next = funcs;
                                funcs = next;
                                next->fullname = strdup(sqlite3_column_text(handle, 0));
                                rc = SQLITE_OK;
                                break;
                            }
                            default:
                                done = TRUE;
                                break;
                        }
                    }
                    finalize(handle);
                }
            }
            for (i = 0; i < count; i++)
            {
                if (ids[count] != -1)
                {
                    rc = prepare(db, query2, strlen(query2) + 1, &handle, NULL);
                    if (rc == SQLITE_OK)
                    {
                        int done = FALSE;
                        rc = SQLITE_DONE;
                        sqlite3_reset(handle);
                        sqlite3_bind_int(handle, 1, ids[i]);
                        sqlite3_bind_text(handle, 2, file, strlen(file), SQLITE_STATIC);
                        while (!done)
                        {
                            switch (rc = sqlite3_step(handle))
                            {
                                case SQLITE_BUSY:
                                    done = TRUE;
                                    break;
                                case SQLITE_DONE:
                                    done = TRUE;
                                    break;
                                case SQLITE_ROW:
                                {
                                    CCFUNCDATA* next = calloc(1, sizeof(CCFUNCDATA));
                                    ids[count] = -1;
                                    next->next = funcs;
                                    funcs = next;
                                    next->fullname = strdup(sqlite3_column_text(handle, 0));
                                    rc = SQLITE_OK;
                                    break;
                                }
                                default:
                                    done = TRUE;
                                    break;
                            }
                        }
                        finalize(handle);
                    }
                }
            }
        }
        scan = &funcs;
        while (*scan)
        {
            if (ccLookupFunctionType((*scan)->fullname, file, &id, &baseid) && (args = ccLookupArgList(id, baseid)))
            {
                (*scan)->args = args;
                scan = &(*scan)->next;
            }
            else
            {
                CCFUNCDATA* toFree = *scan;
                *scan = (*scan)->next;
                free(toFree->fullname);
                free(toFree);
            }
        }
    }
    return funcs;
}
void ccFreeFunctionList(CCFUNCDATA* data)
{
    while (data)
    {
        CCFUNCDATA* next = data->next;
        ccFreeArgList(data->args);
        free(data->fullname);
        free(data);
        data = next;
    }
}
