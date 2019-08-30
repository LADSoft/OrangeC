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
#include <ctype.h>
#include <richedit.h>

#include "header.h"
#include "sqlite3.h"

#define DBVersion 100
static int version_ok;

void DBClose(sqlite3* db);

static int verscallback(void* NotUsed, int argc, char** argv, char** azColName)
{
    (void)NotUsed;
    (void)azColName;
    if (argc == 1)
    {
        if (atoi(argv[0]) >= DBVersion)
            version_ok = TRUE;
    }
    return 0;
}
static DWORD ReadImageBase(sqlite3* db)
{
    static char* query = {"SELECT value FROM dbPropertyBag WHERE property = \"ImageBase\";"};
    int rc = SQLITE_OK;
    DWORD rv = -1;
    sqlite3_stmt* handle;

    rc = sqlite3_prepare_v2(db, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
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
                    rv = atoi((char*)sqlite3_column_text(handle, 0));
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    return rv;
}

DEBUG_INFO* DebugDBOpen(char* name)
{
    sqlite3* db = NULL;
    DEBUG_INFO* rv = NULL;
    if (sqlite3_open_v2(name, &db, SQLITE_OPEN_READWRITE, NULL) == SQLITE_OK)
    {
        char* zErrMsg = NULL;
        version_ok = FALSE;
        if (sqlite3_exec(db, "SELECT value FROM dbPropertyBag WHERE property = \"dbVersion\"", verscallback, 0, &zErrMsg) !=
                SQLITE_OK ||
            !version_ok)
        {
            sqlite3_free(zErrMsg);
        }
        else
        {
            rv = (DEBUG_INFO*)calloc(1, sizeof(DEBUG_INFO));
            if (rv)
            {
                sqlite3_busy_timeout(db, 400);
                rv->dbPointer = db;
                rv->linkbase = ReadImageBase(db);
                if (rv->linkbase == (DWORD)-1)
                {
                    free(rv);
                    rv = NULL;
                }
            }
        }
    }
    if (!rv && db)
        DBClose(db);
    return rv;
}
int GetSymbolAddress(DEBUG_INFO* dbg_info, char* name)
{
    static char* query = {
        "SELECT globals.varAddress FROM globals"
        "    JOIN Names on globals.symbolId = Names.id"
        "    WHERE Names.name = ?;"};
    static char* query2 = {
        "SELECT Locals.varAddress FROM Locals"
        "    JOIN Names on Locals.symbolId = Names.id"
        "    WHERE Names.name = ?;"};
    static char* query3 = {
        "SELECT virtuals.varAddress FROM virtuals"
        "    JOIN Names on virtuals.symbolId = Names.id"
        "    WHERE Names.name = ?;"};
    int rv = 0;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
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
                    rv = sqlite3_column_int(handle, 0);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
        if (rc != SQLITE_OK)
        {
            rc = sqlite3_prepare_v2(dbg_info->dbPointer, query2, strlen(query2) + 1, &handle, NULL);
            if (rc == SQLITE_OK)
            {
                int done = FALSE;
                rc = SQLITE_DONE;
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
                            rv = sqlite3_column_int(handle, 0);
                            rc = SQLITE_OK;
                            done = TRUE;
                            break;
                        default:
                            done = TRUE;
                            break;
                    }
                }
            }
            sqlite3_finalize(handle);
        }
        if (rc != SQLITE_OK)
        {
            rc = sqlite3_prepare_v2(dbg_info->dbPointer, query3, strlen(query3) + 1, &handle, NULL);
            if (rc == SQLITE_OK)
            {
                int done = FALSE;
                rc = SQLITE_DONE;
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
                            rv = sqlite3_column_int(handle, 0);
                            rc = SQLITE_OK;
                            done = TRUE;
                            break;
                        default:
                            done = TRUE;
                            break;
                    }
                }
            }
            sqlite3_finalize(handle);
        }
    }
    return rv;
}
int GetEqualsBreakpoint(DEBUG_INFO* dbg_info, DWORD Address, char* module, int* linenum)
{
    static char* query = {
        "SELECT FileNames.name, LineNumbers.line, LineNumbers.address FROM  LineNumbers"
        "    JOIN FileNames on LineNumbers.fileId = FileNames.id"
        "    WHERE LineNumbers.address <= ? ORDER BY LineNumbers.address DESC ;"};
    int rv = 0;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_int(handle, 1, Address);
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
                    // skim to last listed line number...
                    if (rv == 0 || sqlite3_column_int(handle, 2) == rv)
                    {
                        strcpy(module, (char*)sqlite3_column_text(handle, 0));
                        *linenum = sqlite3_column_int(handle, 1);
                        if (!rv)
                            rv = sqlite3_column_int(handle, 2);
                    }
                    else
                    {
                        rc = SQLITE_OK;
                        done = TRUE;
                    }
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    if (!rv || GetFuncId(dbg_info, Address) != GetFuncId(dbg_info, rv))
    {
        *linenum = 0;
        return 0;
    }
    return rv;
}
int GetHigherBreakpoint(DEBUG_INFO* dbg_info, DWORD Address, char* module, int* linenum)
{
    static char* query = {
        "SELECT FileNames.name, LineNumbers.line, LineNumbers.address FROM  LineNumbers"
        "    JOIN FileNames on LineNumbers.fileId = FileNames.id"
        "    WHERE LineNumbers.address > ? ORDER BY LineNumbers.address ASC;"};
    int rv = 0;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_int(handle, 1, Address);
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
                    // skim to last listed line number...
                    if (rv == 0 || sqlite3_column_int(handle, 2) == rv)
                    {
                        strcpy(module, (char*)sqlite3_column_text(handle, 0));
                        *linenum = sqlite3_column_int(handle, 1);
                        if (!rv)
                            rv = sqlite3_column_int(handle, 2);
                    }
                    else
                    {
                        rc = SQLITE_OK;
                        done = TRUE;
                    }
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    if (!rv || GetFuncId(dbg_info, Address) != GetFuncId(dbg_info, rv))
    {
        *linenum = 0;
        return 0;
    }
    return rv;
}
void GetBreakpointAddressesInternal(DEBUG_INFO* dbg_info, char* module, int* linenum, int** array, int* max, int* count)
{
    static char* query = {
        "SELECT LineNumbers.line, LineNumbers.address FROM  LineNumbers"
        "    JOIN FileNames on LineNumbers.fileId = FileNames.id"
        "    WHERE FileNames.name = ? AND"
        "       LineNumbers.line = ? ORDER BY LineNumbers.line ASC;"};
    char myName[MAX_PATH];
    int rc = SQLITE_OK;
    char *p = module, *q = myName;
    sqlite3_stmt* handle;
    while (*p)
        *q++ = tolower(*p++);
    *q = 0;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_text(handle, 1, myName, strlen(myName), SQLITE_STATIC);
        sqlite3_bind_int(handle, 2, *linenum);
        while (!done)
        {
            switch (rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                case SQLITE_ROW:
                    if (*count == 0)
                    {
                        *linenum = sqlite3_column_int(handle, 0);
                        *max = 20;
                        *array = realloc(*array, *max * sizeof(int));
                        if (array)
                        {
                            (*array)[(*count)++] = sqlite3_column_int(handle, 1) + dbg_info->loadbase - dbg_info->linkbase;
                        }
                        else
                        {
                            done = TRUE;
                        }
                    }
                    else if (*linenum == sqlite3_column_int(handle, 0))
                    {
                        if (*count >= *max)
                        {
                            *max *= 2;
                            *array = realloc(*array, *max * sizeof(int));
                        }
                        if (*array)
                        {
                            (*array)[(*count)++] = sqlite3_column_int(handle, 1) + dbg_info->loadbase - dbg_info->linkbase;
                        }
                        else
                        {
                            done = TRUE;
                        }
                    }
                    else
                    {
                        rc = SQLITE_OK;
                        done = TRUE;
                    }
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
}
int GetHigherBreakpointLineByModule(DEBUG_INFO* dbg_info, char* module, int linenum)
{
    static char* query = {
        "SELECT LineNumbers.line FROM  LineNumbers"
        "    JOIN FileNames on LineNumbers.fileId = FileNames.id"
        "    WHERE FileNames.name = ? AND"
        "       LineNumbers.line >= ? ORDER BY LineNumbers.line ASC;"};
    int rv = 0;
    char myName[MAX_PATH];
    int rc = SQLITE_OK;
    char *p = module, *q = myName;
    sqlite3_stmt* handle;
    while (*p)
        *q++ = tolower(*p++);
    *q = 0;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_text(handle, 1, myName, strlen(myName), SQLITE_STATIC);
        sqlite3_bind_int(handle, 2, linenum);
        while (!done)
        {
            switch (rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                case SQLITE_ROW:
                    rv = sqlite3_column_int(handle, 0);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    return rv;
}
#define LT_MAX 100
int* GetLineTableInternal(DEBUG_INFO* dbg_info, char* name, int lineno, int* lc)
{
    char myName[MAX_PATH];
    int* rv = calloc(LT_MAX, sizeof(int));
    int count = 0;
    static char* query = {
        "SELECT LineNumbers.line FROM  LineNumbers"
        "    JOIN FileNames on LineNumbers.fileId = FileNames.id"
        "    WHERE FileNames.name = ? AND"
        "       LineNumbers.line >= ? ORDER BY LineNumbers.line ASC;"};
    int rc = SQLITE_OK;
    char *p = name, *q = myName;
    sqlite3_stmt* handle;
    while (*p)
        *q++ = tolower(*p++);
    *q = 0;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_text(handle, 1, myName, strlen(myName), SQLITE_STATIC);
        sqlite3_bind_int(handle, 2, lineno);
        while (!done)
        {
            switch (rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                case SQLITE_ROW:
                    rv[count] = sqlite3_column_int(handle, 0);
                    if (++count >= LT_MAX)
                    {
                        rc = SQLITE_OK;
                        done = TRUE;
                    }
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    if (count == 0)
    {
        free(rv);
        rv = NULL;
    }
    else
    {
        *lc = count;
    }
    return rv;
}
int GetGlobalName(DEBUG_INFO* dbg_info, char* name, int* type, int Address, int equals)
{
    char gname[512];
    int gtype;
    int gaddr;
    char lname[512];
    int ltype;
    int laddr;
    char vname[512];
    int vtype;
    int vaddr;
    static char* eqquery = {
        "SELECT Names.name, globals.varAddress, globals.typeId FROM Names"
        "    JOIN globals on globals.symbolId = Names.id"
        "    WHERE globals.varAddress = ?"
        "       ORDER BY globals.varAddress DESC;"};
    static char* lequery = {
        "SELECT Names.name, globals.varAddress, globals.typeId FROM Names"
        "    JOIN globals on globals.symbolId = Names.id"
        "    WHERE globals.varAddress <= ?"
        "       ORDER BY globals.varAddress DESC;"};
    static char* leqquery = {
        "SELECT Names.name, Locals.varAddress, Locals.typeId FROM Names"
        "    JOIN Locals on Locals.symbolId = Names.id"
        "    WHERE Locals.varAddress = ?"
        "       ORDER BY Locals.varAddress DESC;"};
    static char* llequery = {
        "SELECT Names.name, Locals.varAddress, Locals.typeId FROM Names"
        "    JOIN Locals on Locals.symbolId = Names.id"
        "    WHERE Locals.varAddress <= ?"
        "       ORDER BY Locals.varAddress DESC;"};
    static char* veqquery = {
        "SELECT Names.name, virtuals.varAddress, virtuals.typeId FROM Names"
        "    JOIN virtuals on virtuals.symbolId = Names.id"
        "    WHERE virtuals.varAddress = ?"
        "       ORDER BY virtuals.varAddress DESC;"};
    static char* vlequery = {
        "SELECT Names.name, virtuals.varAddress, virtuals.typeId FROM Names"
        "    JOIN virtuals on virtuals.symbolId = Names.id"
        "    WHERE virtuals.varAddress <= ?"
        "       ORDER BY virtuals.varAddress DESC;"};
    char* query = equals ? eqquery : lequery;
    int rv = 0;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    gname[0] = lname[0] = 0;
    gaddr = laddr = vaddr = 0;
    gtype = ltype = vtype = 0;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_int(handle, 1, Address);
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
                    strcpy(gname, (char*)sqlite3_column_text(handle, 0));
                    gaddr = sqlite3_column_int(handle, 1);
                    gtype = sqlite3_column_int(handle, 2);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    query = equals ? leqquery : llequery;
    rc = SQLITE_OK;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_int(handle, 1, Address);
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
                    strcpy(lname, (char*)sqlite3_column_text(handle, 0));
                    laddr = sqlite3_column_int(handle, 1);
                    ltype = sqlite3_column_int(handle, 2);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    query = equals ? veqquery : vlequery;
    rc = SQLITE_OK;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_int(handle, 1, Address);
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
                    strcpy(vname, (char*)sqlite3_column_text(handle, 0));
                    vaddr = sqlite3_column_int(handle, 1);
                    vtype = sqlite3_column_int(handle, 2);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    if (gaddr < laddr)
    {
        if (vaddr < laddr)
        {
            strcpy(name, lname);
            if (type)
                *type = ltype;
            return laddr;
        }
        else
        {
            strcpy(name, vname);
            if (type)
                *type = vtype;
            return vaddr;
        }
    }
    else if (gaddr < vaddr)
    {
        strcpy(name, vname);
        if (type)
            *type = vtype;
        return vaddr;
    }
    else
    {
        strcpy(name, gname);
        if (type)
            *type = gtype;
        return gaddr;
    }
}
int GetFuncId(DEBUG_INFO* dbg_info, int Address)
{
    static char* query = {
        "SELECT Names.id, globals.varAddress FROM Names"
        "    JOIN globals on globals.symbolId = Names.id"
        "    WHERE globals.varAddress <= ?"
        "       ORDER BY globals.varAddress DESC;"};
    static char* lquery = {
        "SELECT Names.id, Locals.varAddress FROM Names"
        "    JOIN Locals on Locals.symbolId = Names.id"
        "    WHERE Locals.varAddress <= ?"
        "       ORDER BY Locals.varAddress DESC;"};
    static char* vquery = {
        "SELECT Names.id, virtuals.varAddress FROM Names"
        "    JOIN virtuals on virtuals.symbolId = Names.id"
        "    WHERE virtuals.varAddress <= ?"
        "       ORDER BY virtuals.varAddress DESC;"};
    int rv = 0, gbl = 0, lcl = 0, vir = 0, agbl = 0, alcl = 0, avir = 0;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_int(handle, 1, Address);
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
                    gbl = sqlite3_column_int(handle, 0);
                    agbl = sqlite3_column_int(handle, 1);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, lquery, strlen(lquery) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_int(handle, 1, Address);
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
                    lcl = sqlite3_column_int(handle, 0);
                    alcl = sqlite3_column_int(handle, 1);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, vquery, strlen(vquery) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_int(handle, 1, Address);
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
                    vir = sqlite3_column_int(handle, 0);
                    avir = sqlite3_column_int(handle, 1);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    if (agbl < alcl)
        if (avir < alcl)
            return lcl;
        else
            return vir;
    else if (agbl < avir)
        return vir;
    else
        return gbl;
}
NAMELIST* GetEnclosedAutos(DEBUG_INFO* dbg_info, int funcId, int line)
{
    static char* query = {
        "SELECT Names.name FROM Names"
        "    JOIN Autos on Autos.symbolId = Names.id"
        "    WHERE Autos.funcId = ?"
        "       AND Autos.beginLine <= ?"
        "       And Autos.endLine >= ?"
        "       ORDER BY Autos.beginLine DESC;"};
    NAMELIST* rv = NULL;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_int(handle, 1, funcId);
        sqlite3_bind_int(handle, 2, line);
        sqlite3_bind_int(handle, 3, line);
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
                    const unsigned char* p = sqlite3_column_text(handle, 0);
                    if (p && p[0])
                    {
                        NAMELIST* next = calloc(1, sizeof(NAMELIST));
                        next->data = strdup(p);
                        next->next = rv;
                        rv = next;
                    }
                    rc = SQLITE_OK;
                    break;
                }
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    return rv;
}
int GetLocalSymbolAddress(DEBUG_INFO* dbg_info, int funcId, char* name, char* filename, int line, int* address, int* type)
{
    static char* query = {
        "SELECT Autos.varOffset, Autos.typeId FROM Autos"
        "    JOIN Names on Autos.symbolId = Names.id"
        "    WHERE Names.name = ?"
        "       AND Autos.funcId = ?"
        "       AND Autos.beginLine <= ?"
        "       And Autos.endLine >= ?"
        "       ORDER BY Autos.beginLine DESC;"};
    char iname[256];
    int rv = 0;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    sprintf(iname, "_%s", name);
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_text(handle, 1, iname, strlen(iname), SQLITE_STATIC);
        sqlite3_bind_int(handle, 2, funcId);
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
                    *address = sqlite3_column_int(handle, 0);
                    *type = sqlite3_column_int(handle, 1);
                    rv = TRUE;
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    return rv;
}
int GetGlobalSymbolAddress(DEBUG_INFO* dbg_info, char* name, char* filename, int line, int* address, int* type)
{
    static char* loquery = {
        "SELECT Locals.varAddress, Locals.typeId FROM Locals"
        "    JOIN Names on Locals.symbolId = Names.id"
        "    JOIN FileNames on Locals.fileId = FileNames.id"
        "    WHERE (Names.name = ? OR Names.Name = ?)"
        "       AND FileNames.name = ?"};
    char iname[256];
    int rv = 0;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    sprintf(iname, "_%s", name);
    // search the locals for this file first
    if (filename[0])
    {
        char myName[MAX_PATH];
        char *p = filename, *q = myName;
        while (*p)
            *q++ = tolower(*p++);
        *q = 0;
        rc = sqlite3_prepare_v2(dbg_info->dbPointer, loquery, strlen(loquery) + 1, &handle, NULL);
        if (rc == SQLITE_OK)
        {
            int done = FALSE;
            rc = SQLITE_DONE;
            sqlite3_bind_text(handle, 1, iname, strlen(iname), SQLITE_STATIC);
            sqlite3_bind_text(handle, 2, name, strlen(name), SQLITE_STATIC);
            sqlite3_bind_text(handle, 3, myName, strlen(myName), SQLITE_STATIC);
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
                        *address = sqlite3_column_int(handle, 0);
                        *type = sqlite3_column_int(handle, 1);
                        rv = TRUE;
                        rc = SQLITE_OK;
                        done = TRUE;
                        break;
                    default:
                        done = TRUE;
                        break;
                }
            }
            sqlite3_finalize(handle);
        }
    }
    // if it wasn't in the locals for this file, search the globals, unqualified
    if (!rv)
    {
        static char* glquery = {
            "SELECT globals.varAddress, globals.typeId FROM globals"
            "    JOIN Names on globals.symbolId = Names.id"
            "    WHERE Names.name = ? OR Names.Name = ?;"};
        int rc = SQLITE_OK;
        sqlite3_stmt* handle;
        rc = sqlite3_prepare_v2(dbg_info->dbPointer, glquery, strlen(glquery) + 1, &handle, NULL);
        if (rc == SQLITE_OK)
        {
            int done = FALSE;
            rc = SQLITE_DONE;
            sqlite3_bind_text(handle, 1, iname, strlen(iname), SQLITE_STATIC);
            sqlite3_bind_text(handle, 2, name, strlen(name), SQLITE_STATIC);
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
                        *address = sqlite3_column_int(handle, 0);
                        *type = sqlite3_column_int(handle, 1);
                        rv = TRUE;
                        rc = SQLITE_OK;
                        done = TRUE;
                        break;
                    default:
                        done = TRUE;
                        break;
                }
            }
            sqlite3_finalize(handle);
        }
    }
    // now search the globals
    if (!rv)
    {
        static char* virquery = {
            "SELECT virtuals.varAddress, virtuals.typeId FROM virtuals"
            "    JOIN Names on virtuals.symbolId = Names.id"
            "    WHERE Names.name = ? OR Names.Name = ?;"};
        int rc = SQLITE_OK;
        sqlite3_stmt* handle;
        rc = sqlite3_prepare_v2(dbg_info->dbPointer, virquery, strlen(virquery) + 1, &handle, NULL);
        if (rc == SQLITE_OK)
        {
            int done = FALSE;
            rc = SQLITE_DONE;
            sqlite3_bind_text(handle, 1, iname, strlen(iname), SQLITE_STATIC);
            sqlite3_bind_text(handle, 2, name, strlen(name), SQLITE_STATIC);
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
                        *address = sqlite3_column_int(handle, 0);
                        *type = sqlite3_column_int(handle, 1);
                        rv = TRUE;
                        rc = SQLITE_OK;
                        done = TRUE;
                        break;
                    default:
                        done = TRUE;
                        break;
                }
            }
            sqlite3_finalize(handle);
        }
    }
    // if we didn't find it in the locals for the file or the globals, search the locals again without qualifiers
    // and take the first match
    if (!rv)
    {
        static char* glquery = {
            "SELECT Locals.varAddress, Locals.typeId FROM Locals"
            "    JOIN Names on Locals.symbolId = Names.id"
            "    WHERE Names.name = ? OR Names.Name = ?;"};
        int rc = SQLITE_OK;
        sqlite3_stmt* handle;
        rc = sqlite3_prepare_v2(dbg_info->dbPointer, glquery, strlen(glquery) + 1, &handle, NULL);
        if (rc == SQLITE_OK)
        {
            int done = FALSE;
            rc = SQLITE_DONE;
            sqlite3_bind_text(handle, 1, iname, strlen(iname), SQLITE_STATIC);
            sqlite3_bind_text(handle, 2, name, strlen(name), SQLITE_STATIC);
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
                        *address = sqlite3_column_int(handle, 0);
                        *type = sqlite3_column_int(handle, 1);
                        rv = TRUE;
                        rc = SQLITE_OK;
                        done = TRUE;
                        break;
                    default:
                        done = TRUE;
                        break;
                }
            }
            sqlite3_finalize(handle);
        }
    }
    return rv;
}
int GetType(DEBUG_INFO* dbg_info, char* name)
{
    static char* query = {
        "SELECT TypeNames.typeId from typeNames"
        "    JOIN Names on TypeNames.symbolId = Names.id"
        "    WHERE Names.name = ?;"};
    int rv = 0;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
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
                    rv = sqlite3_column_int(handle, 0);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    return rv;
}
int GetTypeName(DEBUG_INFO* dbg_info, int type, char* name)
{
    static char* query = {
        "SELECT Types.name from types"
        "    WHERE Types.id = ?"};
    int rv = 0;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_int(handle, 1, type);
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
                    const unsigned char* p = sqlite3_column_text(handle, 0);
                    if (p && p[0])
                    {
                        strcpy(name, (char*)p);
                        rv = 1;
                    }
                    else
                    {
                        name[0] = 0;
                    }
                    rc = SQLITE_OK;
                    done = TRUE;
                }
                break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    return rv;
}
int GetReturnTypeQualifier(DEBUG_INFO* dbg_info, int type)
{
    static char* query = {
        "SELECT Types.baseType from types"
        "    WHERE Types.id = ?"};
    int rv = 0;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_int(handle, 1, type);
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
                    rv = sqlite3_column_int(handle, 0);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    return rv;
}
int GetTypeQualifier(DEBUG_INFO* dbg_info, int type)
{
    static char* query = {
        "SELECT Types.qualifier from types"
        "    WHERE Types.id = ?"};
    int rv = 0;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_int(handle, 1, type);
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
                    rv = sqlite3_column_int(handle, 0);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    return rv;
}
FUNCTIONLIST* LookupCPPNames(DEBUG_INFO* dbg_info, char* name, FUNCTIONLIST* in)
{
    static char* query = {
        "SELECT id From Names"
        "     Where name = ?"};
    static char* query1 = {
        "SELECT complexId FROM CPPNameMapping"
        "    WHERE simpleId = ?"};
    static char* query2 = {

        "SELECT name From Names"
        "     Where id = ?"};
    static char* query3 = {
        "SELECT varAddress From globals"
        "    Where symbolId = ?"};
    static char* query4 = {
        "SELECT varAddress From virtuals"
        "    Where symbolId = ?"};

    int id = -1;
    int rv = 0;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
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
                    id = sqlite3_column_int(handle, 0);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    if (id >= 0)
    {
        rc = sqlite3_prepare_v2(dbg_info->dbPointer, query1, strlen(query1) + 1, &handle, NULL);
        if (rc == SQLITE_OK)
        {
            int done = FALSE;
            rc = SQLITE_DONE;
            sqlite3_bind_int(handle, 1, id);
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
                        FUNCTIONLIST* next = calloc(sizeof(FUNCTIONLIST), 1);
                        next->address = -sqlite3_column_int(handle, 0);
                        next->next = in;
                        in = next;
                        break;
                    }
                    default:
                        done = TRUE;
                        break;
                }
            }
            sqlite3_finalize(handle);
        }
        if (rc == SQLITE_OK)
        {
            FUNCTIONLIST* scan = in;
            while (scan)
            {
                if (scan->address < 0)
                {
                    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query2, strlen(query2) + 1, &handle, NULL);
                    if (rc == SQLITE_OK)
                    {
                        int done = FALSE;
                        rc = SQLITE_DONE;
                        sqlite3_bind_int(handle, 1, -scan->address);
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
                                    strcpy(scan->name, (char*)sqlite3_column_text(handle, 0));
                                    rc = SQLITE_OK;
                                    done = TRUE;
                                    break;
                                default:
                                    done = TRUE;
                                    break;
                            }
                        }
                        sqlite3_finalize(handle);
                    }
                }
                scan = scan->next;
            }
            scan = in;
            while (scan)
            {
                if (scan->address < 0)
                {
                    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query3, strlen(query3) + 1, &handle, NULL);
                    if (rc == SQLITE_OK)
                    {
                        int done = FALSE;
                        rc = SQLITE_DONE;
                        sqlite3_bind_int(handle, 1, -scan->address);
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
                                    scan->address = sqlite3_column_int(handle, 0);
                                    rc = SQLITE_OK;
                                    done = TRUE;
                                    break;
                                default:
                                    done = TRUE;
                                    break;
                            }
                        }
                        sqlite3_finalize(handle);
                    }
                }
                scan = scan->next;
            }
            scan = in;
            while (scan)
            {
                if (scan->address < 0)
                {
                    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query4, strlen(query4) + 1, &handle, NULL);
                    if (rc == SQLITE_OK)
                    {
                        int done = FALSE;
                        rc = SQLITE_DONE;
                        sqlite3_bind_int(handle, 1, -scan->address);
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
                                    scan->address = sqlite3_column_int(handle, 0);
                                    rc = SQLITE_OK;
                                    done = TRUE;
                                    break;
                                default:
                                    done = TRUE;
                                    break;
                            }
                        }
                        sqlite3_finalize(handle);
                    }
                }
                scan = scan->next;
            }
        }
    }

    return in;
}
int LookupTypedefValues(DEBUG_INFO* dbg_info, int type)
{
    static char* query = {
        "SELECT baseType FROM Types"
        "    WHERE Types.id = ?"};
    int rv = 0;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_int(handle, 1, type);
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
                    rv = sqlite3_column_int(handle, 0);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    return rv;
}
int LookupPointerSubtype(DEBUG_INFO* dbg_info, int type)
{
    static char* query = {
        "SELECT baseType FROM Types"
        "    WHERE Types.id = ?"};
    int rv = 0;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_int(handle, 1, type);
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
                    rv = sqlite3_column_int(handle, 0);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    return rv;
}
int LookupBitfieldInfo(DEBUG_INFO* dbg_info, int type, int* start, int* end)
{
    static char* query = {
        "SELECT baseType, startBit, bitCount FROM Types"
        "    WHERE Types.id = ?"};
    int rv = 0;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_int(handle, 1, type);
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
                    rv = sqlite3_column_int(handle, 0);
                    *start = sqlite3_column_int(handle, 1);
                    *end = sqlite3_column_int(handle, 2);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    return rv;
}
VARINFO* LookupStructInfo(DEBUG_INFO* dbg_info, int type, int Address, char* structtag, int* size)
{
    static char* query = {
        "SELECT Types.size, Names.name FROM Types"
        "   JOIN TypeNames on TypeNames.typeId = types.id"
        "   JOIN Names on Names.id = TypeNames.symbolId"
        "    WHERE Types.id = ?;"};
    static char* fieldquery = {
        "SELECT Fields.baseType, Fields.offsetOrOrd, Fields.fieldOrder, Names.name FROM Fields"
        "   JOIN Names on Names.id = Fields.symbolId"
        "    WHERE Fields.typeId = ?"
        "       ORDER BY fieldOrder ASC;"};
    VARINFO *rv = NULL, **next = &rv;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_int(handle, 1, type);
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
                    *size = sqlite3_column_int(handle, 0);
                    strcpy(structtag, (char*)sqlite3_column_text(handle, 1));
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, fieldquery, strlen(fieldquery) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_int(handle, 1, type);
        while (!done)
        {
            switch (rc = sqlite3_step(handle))
            {
                case SQLITE_BUSY:
                    done = TRUE;
                    break;
                case SQLITE_DONE:
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                case SQLITE_ROW:
                    *next = calloc(1, sizeof(VARINFO));
                    if (*next)
                    {
                        (*next)->offset = sqlite3_column_int(handle, 1);
                        (*next)->address = Address + (*next)->offset;
                        (*next)->type = sqlite3_column_int(handle, 0);
                        if ((*next)->type == type)
                            (*next)->size = *size;
                        else
                            (*next)->size = DeclType(dbg_info, (*next));
                        strcpy((*next)->membername, (char*)sqlite3_column_text(handle, 3));
                        next = &(*next)->link;
                    }
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }

    return rv;
}
int LookupArrayInfo(DEBUG_INFO* dbg_info, int type, int* size)
{
    // we are just ignoring all the other info for now...
    static char* query = {
        "SELECT baseType, size FROM Types"
        "    WHERE Types.id = ?"};
    int rv = 0;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_int(handle, 1, type);
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
                    rv = sqlite3_column_int(handle, 0);
                    *size = sqlite3_column_int(handle, 1);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    return rv;
}
int LookupEnumValues(DEBUG_INFO* dbg_info, int type, char* structtag)
{
    static char* query = {
        "SELECT Types.baseType, Names.name FROM Types"
        "   JOIN TypeNames on TypeNames.typeId = types.id"
        "   JOIN Names on TypeNames.symbolId = names.id"
        "    WHERE Types.id = ?;"};
    int rv = 0;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_int(handle, 1, type);
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
                    rv = sqlite3_column_int(handle, 0);
                    strcpy(structtag, (char*)sqlite3_column_text(handle, 1));
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    return rv;
}
int LookupEnumValue(DEBUG_INFO* dbg_info, int type, char* name)
{
    static char* query = {
        "SELECT offsetOrOrd FROM Fields"
        "   JOIN Names on Names.id = fields.symbolId"
        "    WHERE Fields.typeId = ?"
        "       AND Names.name = ?;"};
    int rv = 0;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_int(handle, 1, type);
        sqlite3_bind_text(handle, 2, name, strlen(name), SQLITE_STATIC);
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
                    rv = sqlite3_column_int(handle, 0);
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
    return rv;
}
void LookupEnumName(DEBUG_INFO* dbg_info, int type, char* name, int ord)
{
    static char* query = {
        "SELECT Names.name FROM Names"
        "   JOIN Fields on Names.id = Fields.symbolId"
        "    WHERE Fields.typeId = ? AND"
        "       Fields.offsetOrOrd = ?;"};
    int rv = 0;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(dbg_info->dbPointer, query, strlen(query) + 1, &handle, NULL);
    if (rc == SQLITE_OK)
    {
        int done = FALSE;
        rc = SQLITE_DONE;
        sqlite3_bind_int(handle, 1, type);
        sqlite3_bind_int(handle, 2, ord);
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
                    strcpy(name, (char*)sqlite3_column_text(handle, 0));
                    rc = SQLITE_OK;
                    done = TRUE;
                    break;
                default:
                    done = TRUE;
                    break;
            }
        }
        sqlite3_finalize(handle);
    }
}