/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

#include <windows.h>
#include <stdio.h>
#include "PEHeader.h"
#include "sqlite3.h"

char* unmangle(char* val, char* name);

#define DBVersion 100

int version_ok;
static BOOL DebugFileName(char* buf, BYTE* base)
{
    DWORD dbgBase;
    DWORD read;
    dbgBase = *(DWORD*)(base + 0x3c);
    dbgBase = ((struct PEHeader*)(base + dbgBase))->debug_rva;
    if (dbgBase && !memcmp(base + dbgBase, "LS14", 4))
    {
        memcpy(buf, base + dbgBase + 33, *(BYTE*)(base + dbgBase + 32));
        buf[*(BYTE*)(base + dbgBase + 32)] = 0;
        return 1;
    }
    return 0;
}

static int verscallback(void* NotUsed, int argc, char** argv, char** azColName)
{
    int i;
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

static void DBClose(sqlite3* db)
{
    if (db)
        sqlite3_close(db);
}

static sqlite3* DBOpen(char* name, DWORD* linkbase)
{
    sqlite3* db = NULL;
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
            sqlite3_busy_timeout(db, 400);
            *linkbase = ReadImageBase(db);
            if (*linkbase == (DWORD)-1)
            {

                DBClose(db);
                db = NULL;
            }
        }
    }
    return db;
}
static int GetFuncId(sqlite3* db, int Address)
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
    rc = sqlite3_prepare_v2(db, query, strlen(query) + 1, &handle, NULL);
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
    rc = sqlite3_prepare_v2(db, lquery, strlen(lquery) + 1, &handle, NULL);
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
    rc = sqlite3_prepare_v2(db, vquery, strlen(vquery) + 1, &handle, NULL);
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
    {
        if (avir < alcl)
        {
            return lcl;
        }
        return vir;
    }
    if (agbl < avir)
        return vir;
    return gbl;
}
static int GetEqualsBreakpoint(sqlite3* db, DWORD Address, char* module, int* linenum)
{
    static char* query = {
        "SELECT FileNames.name, LineNumbers.line, LineNumbers.address FROM  LineNumbers"
        "    JOIN FileNames on LineNumbers.fileId = FileNames.id"
        "    WHERE LineNumbers.address <= ? ORDER BY LineNumbers.address DESC ;"};
    int rv = 0;
    int rc = SQLITE_OK;
    sqlite3_stmt* handle;
    rc = sqlite3_prepare_v2(db, query, strlen(query) + 1, &handle, NULL);
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
    if (!rv || GetFuncId(db, Address) != GetFuncId(db, rv))
    {
        *linenum = 0;
        return 0;
    }
    return rv;
}
static int GetGlobalName(sqlite3* db, char* name, int* type, int Address, int equals)
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
    rc = sqlite3_prepare_v2(db, query, strlen(query) + 1, &handle, NULL);
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
    rc = sqlite3_prepare_v2(db, query, strlen(query) + 1, &handle, NULL);
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
    rc = sqlite3_prepare_v2(db, query, strlen(query) + 1, &handle, NULL);
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
void GetModuleName(char* buf, unsigned addr, unsigned* base)
{
    buf[0] = 0;
    HMODULE module;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)addr, &module))
    {
        DWORD dbgBase;
        DWORD read;
        *base = (unsigned)module;
        dbgBase = *(DWORD*)((BYTE*)module + 0x3c);
        dbgBase = ((struct PEHeader*)((BYTE*)module + dbgBase))->export_rva;
        if (dbgBase)
        {
            dbgBase = *(DWORD*)((BYTE*)module + dbgBase + 12);
            strcpy(buf, (char*)((BYTE*)module + dbgBase));
        }
    }
}
BOOL IsConsoleApp()
{
    BYTE* base = 0x400000;
    DWORD dbgBase;
    DWORD read;
    dbgBase = *(DWORD*)(base + 0x3c);
    return ((struct PEHeader*)(base + dbgBase))->subsystem == PE_SUBSYS_CONSOLE;
}
__declspec(dllexport) void CALLBACK StackTraceWithCode(char* text, char* prog, PCONTEXT regs, void* base, void* stacktop, void *codestart, void *codeend)
{
    sqlite3* db = NULL;
    unsigned currentBase = 0;
    DWORD linkbase = 0;
    char buf[50000];
    sprintf(buf, "\n%s:(%s)\n", text, prog);
    sprintf(buf + strlen(buf), "CS:EIP %04X:%08X  SS:ESP %04X:%08X\n", regs->SegCs, regs->Eip, regs->SegSs, regs->Esp);
    sprintf(buf + strlen(buf), "EAX: %08X  EBX: %08X  ECX: %08X  EDX: %08X  flags: %08X\n", regs->Eax, regs->Ebx, regs->Ecx,
            regs->Edx, regs->EFlags);
    sprintf(buf + strlen(buf), "EBP: %08X  ESI: %08X  EDI: %08X\n", regs->Ebp, regs->Esi, regs->Edi);
    sprintf(buf + strlen(buf), " DS:     %04X   ES:     %04X   FS:     %04X   GS:     %04X\n", regs->SegDs, regs->SegEs,
            regs->SegFs, regs->SegGs);

    if (regs->Eip < 0x80000000)
    {
        sprintf(buf + strlen(buf), "\n\nCS:EIP  ");
        for (int i = 0; i < 16; i++)
            sprintf(buf + strlen(buf), "%02X ", ((unsigned char*)regs->Eip)[i]);
    }
    sprintf(buf + strlen(buf), "\n");

    char dbname[MAX_PATH];
    DWORD* sp = regs->Ebp;

    unsigned xbase = 0;
    char name[4096];
    char unmangled[4096];
    int type;
    strcat(buf, "\nStack trace:\n");
    GetModuleName(name, regs->Eip, &xbase);
    if (xbase != currentBase)
    {
        DBClose(db);
        if (DebugFileName(dbname, (BYTE*)xbase))
        {
            db = DBOpen(dbname, &linkbase);
        }
        else
        {
            db = NULL;
        }
        currentBase = xbase;
    }
    sprintf(buf + strlen(buf), "\t%15s ", name);
    sprintf(buf + strlen(buf), "%x", regs->Eip);
    DWORD funcaddr = GetGlobalName(db, name, &type, regs->Eip - xbase + linkbase, 0);
    if (funcaddr)
    {
        unmangle(unmangled, name);
        sprintf(buf + strlen(buf), ": %s + 0x%x", unmangled, regs->Eip - xbase + linkbase - funcaddr);
        int linenum = 0;
        GetEqualsBreakpoint(db, regs->Eip - xbase + linkbase, name, &linenum);
        if (linenum)
        {
            char* p = strrchr(name, '\\');
            if (p)
                p++;
            else
                p = name;
            sprintf(buf + strlen(buf), "  module: %s, line: %d", p, linenum);
        }
    }
    strcat(buf, "\n");
    while ((DWORD)sp >= regs->Esp /*&&  (DWORD)sp < stacktop && sp[1] >= codestart && sp[1] < codeend*/)
    {
        if (strlen(buf) > sizeof(buf) - 1000)
            break; 
        if (sp[1])
        {
            GetModuleName(name, sp[1], &xbase);
            if (xbase != currentBase)
            {
                DBClose(db);
                if (DebugFileName(dbname, (BYTE*)xbase))
                {
                    db = DBOpen(dbname, &linkbase);
                }
                else
                {
                    db = NULL;
                }
                currentBase = xbase;
            }
            sprintf(buf + strlen(buf), "\t%15s ", name);
            sprintf(buf + strlen(buf), "%x", sp[1]);
            if (db)
            {
                DWORD funcaddr = GetGlobalName(db, name, &type, sp[1] - xbase + linkbase, 0);
                if (funcaddr)
                {
                    unmangle(unmangled, name);
                    sprintf(buf + strlen(buf), ": %s + 0x%x", unmangled, sp[1] - xbase + linkbase - funcaddr);
                    int linenum = 0;
                    // a function taking no args and having no return value will calculate the next line, if we don't subtract 1
                    GetEqualsBreakpoint(db, sp[1] - xbase + linkbase - 1, name, &linenum);
                    if (linenum)
                    {
                        char* p = strrchr(name, '\\');
                        if (p)
                            p++;
                        else
                            p = name;
                        sprintf(buf + strlen(buf), "  module: %s, line: %d", p, linenum);
                    }
                }
            }
            strcat(buf, "\n");
        }
        sp = sp[0];
    }
    DBClose(db);
    if (IsConsoleApp())
    {
        fputs(buf, stderr);
        fflush(stderr);
    }
    else
    {
        MessageBoxA(0, buf, text, 0);
    }
}
__declspec(dllexport) void CALLBACK ConvertSymbolName(const char *name, char *buf)
{
        unmangle(buf, name);
}