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
#include "cvexefmt.h"
#include "pefile.h"
#include "dbgtype.h"
#include "sqlite3.h"
extern PROCESS* activeProcess;
extern enum DebugState uState;

void DBClose(sqlite3* db);

//-------------------------------------------------------------------------

DEBUG_INFO* GetDebugInfo(HANDLE hProcess, DWORD base)
{
    DWORD dbgBase;
    DWORD read;
    if (ReadProcessMemory(hProcess, (LPVOID)(base + 0x3c), &dbgBase, 4, &read))
    {
        if (ReadProcessMemory(hProcess, &((struct pe_header_struct*)(base + dbgBase))->debug_rva, &dbgBase, 4, &read))
        {
            if (dbgBase)
            {
                char buf[512];
                if (ReadProcessMemory(hProcess, (LPVOID)(base + dbgBase), buf, MAX_PATH + 33, &read))
                {
                    if (!memcmp(buf, "LS14", 4))
                    {
                        DEBUG_INFO* rv;
                        buf[33 + (unsigned char)buf[32]] = 0;
                        rv = DebugDBOpen(buf + 33);
                        if (rv)
                            rv->loadbase = base;  // calculate an offset based on load address
                        return rv;
                    }
                }
            }
        }
    }
    return NULL;
}

//-------------------------------------------------------------------------

void FreeDebugInfo(DEBUG_INFO* dbg)
{
    if (dbg)
    {
        DBClose(dbg->dbPointer);
        free(dbg);
    }
}

//-------------------------------------------------------------------------

static void getname(char* buf, unsigned char* cname)
{
    memcpy(buf, cname + 1, cname[0]);
    buf[cname[0]] = 0;
}

//-------------------------------------------------------------------------

DWORD GetMainAddress(DEBUG_INFO* dbg)
{
    int rv;
    if (!dbg)
        return 0;
    rv = GetSymbolAddress(dbg, "_main");
    if (!rv)
        rv = GetSymbolAddress(dbg, "WinMain");
    if (!rv)
        rv = GetSymbolAddress(dbg, "DllMain");
    rv = rv - dbg->linkbase + dbg->loadbase;
    return rv;
}
DEBUG_INFO* findDebug(int Address)
{
    DEBUG_INFO* dbg = activeProcess->dbg_info;
    DLL_INFO* dll = activeProcess->dll_info;
    if (dbg)
    {
        while (dll)
        {
            if ((unsigned)Address >= (unsigned)dll->base && ((unsigned)dll->base > (unsigned)activeProcess->dbg_info->loadbase ||
                                                             (unsigned)Address < (unsigned)activeProcess->dbg_info->loadbase))
                dbg = dll->dbg_info;
            dll = dll->next;
        }
    }
    return dbg;
}
int* GetLineTable(char* name, int line, int* lc)
{
    if (activeProcess)
    {
        DEBUG_INFO* dbg = activeProcess->dbg_info;
        DLL_INFO* dll = activeProcess->dll_info;
        int* rv = NULL;
        if (dbg)
            rv = GetLineTableInternal(dbg, name, line, lc);
        if (!rv)
        {
            while (dll)
            {
                if (dll->dbg_info)
                {
                    rv = GetLineTableInternal(dll->dbg_info, name, line, lc);
                    if (rv)
                        break;
                }
                dll = dll->next;
            }
        }
        return rv;
    }
    return NULL;
}

//-------------------------------------------------------------------------

int GetBreakpointLine(int Address, char* module, int* linenum, BOOL next)
{
    DEBUG_INFO* dbg;
    BREAKPOINT* b;
    int* p;
    for (b = activeProcess->breakpoints.next; b; b = b->next)
    {
        for (p = &b->addresses[0]; *p; p++)
        {
            if (*p == Address && b->linenum)
            {
                strcpy(module, b->module);
                *linenum = b->linenum;
                return Address;
            }
        }
    }

    dbg = findDebug(Address);

    if (!dbg)
        return 0;

    int rv = 0;
    Address = Address - dbg->loadbase + dbg->linkbase;
    if (next)
        rv = GetHigherBreakpoint(dbg, Address, module, linenum);
    else
        rv = GetEqualsBreakpoint(dbg, Address, module, linenum);
    if (rv)
        rv += dbg->loadbase - dbg->linkbase;
    return rv;
}

//-------------------------------------------------------------------------

int* GetBreakpointAddresses(char* module, int* linenum)
{
    int *p = NULL, max = 0, count = 0;
    DEBUG_INFO* dbg = activeProcess->dbg_info;
    DLL_INFO* dll = activeProcess->dll_info;
    if (dbg)
        GetBreakpointAddressesInternal(dbg, module, linenum, &p, &max, &count);
    while (dll)
    {
        if (dll->dbg_info)
        {
            GetBreakpointAddressesInternal(dll->dbg_info, module, linenum, &p, &max, &count);
        }
        dll = dll->next;
    }
    if (count)
    {
        if (count == max)
        {
            p = realloc(p, sizeof(int) * (count + 1));
        }
        p[count] = 0;
    }
    return p;
}

//-------------------------------------------------------------------------

int GetBreakpointNearestLine(char* module, int linenum, int inmodule)
{
    if (uState == notDebugging)
        return linenum;
    if (activeProcess)
    {
        int rv = 0;
        DEBUG_INFO* dbg = activeProcess->dbg_info;
        DLL_INFO* dll = activeProcess->dll_info;
        if (dbg)
            rv = GetHigherBreakpointLineByModule(dbg, module, linenum);
        if (rv)
            return rv;
        while (dll)
        {
            if (dll->dbg_info)
            {
                rv = GetHigherBreakpointLineByModule(dll->dbg_info, module, linenum);
                if (rv)
                    return rv;
            }
            dll = dll->next;
        }
    }
    return -1;
}

void unmanglename(char* buf)
{
    char internal[4096];
    unmangle(internal, buf);
    internal[40] = 0;
    strcpy(buf, internal);
}

FUNCTIONLIST* GetFunctionList(DEBUG_INFO* dbg_info, SCOPE* scope, char* name)
{
    FUNCTIONLIST *rv = NULL, *scan;
    if (!strrchr(name + 1, '@'))
    {
        DEBUG_INFO* inf;
        int address, type;
        if (FindSymbol(&inf, scope, name + 1, &address, &type))
        {
            FUNCTIONLIST* next = calloc(sizeof(FUNCTIONLIST), 1);
            next->next = rv;
            rv = next;
            strcpy(next->name, name + 1);
            next->address = address;
        }
        rv = LookupCPPNames(dbg_info, name + 1, rv);
    }
    rv = LookupCPPNames(dbg_info, name, rv);

    scan = rv;
    while (scan)
    {
        unmanglename(scan->name);
        scan = scan->next;
    }
    return rv;
}
int FindFunctionName(char* buf, int Address, DEBUG_INFO** dbg_out, int* type)
{
    DEBUG_INFO* dbg;
    int rv;

    buf[0] = 0;
    dbg = findDebug(Address);
    if (!dbg)
        return 0;
    Address = Address - dbg->loadbase + dbg->linkbase;
    rv = GetGlobalName(dbg, buf, type, Address, FALSE);
    if (rv)
    {
        unmanglename(buf);
    }
    if (dbg_out)
        *dbg_out = dbg;
    return rv;
}
//-------------------------------------------------------------------------

int FindGlobalSymbol(DEBUG_INFO** dbg_info, int Address, char* buf, int* type)
{
    DEBUG_INFO* dbg;
    int rv;

    buf[0] = 0;
    dbg = findDebug(Address);
    if (!dbg)
        return 0;
    Address = Address - dbg->loadbase + dbg->linkbase;
    rv = GetGlobalName(dbg, buf, type, Address, TRUE);
    if (rv)
    {
        unmanglename(buf);
    }
    return rv;
}

//-------------------------------------------------------------------------
NAMELIST* FindEnclosedAutos(SCOPE* scope)
{
    DEBUG_INFO* dbg;
    int funcId;
    int line;
    int Address;
    char fileName[MAX_PATH];
    if (!scope)
        return 0;
    dbg = findDebug(scope->address);

    if (!dbg)
        return 0;
    Address = scope->address - dbg->loadbase + dbg->linkbase;
    funcId = GetFuncId(dbg, Address);
    if (funcId)
    {
        if (GetBreakpointLine(scope->address, fileName, &line, FALSE))
        {

            return GetEnclosedAutos(dbg, funcId, line);
        }
    }
    return 0;
}
// this does a lot of database lookups but it only happens when the user is entering
// a new value to watch or whatever so it doesn't matter if it is slow...
SCOPE* FindSymbol(DEBUG_INFO** dbg_info, SCOPE* scope, char* name, int* address, int* type)
{
    DEBUG_INFO* dbg;
    SCOPE* rv = scope;
    int funcId;
    int Address;
    char fileName[MAX_PATH];
    int line = 0;
    if (!scope)
        return 0;
    dbg = findDebug(scope->address);

    if (!dbg)
        return 0;

    // walk up the stack...
    while (rv)
    {
        Address = rv->address - dbg->loadbase + dbg->linkbase;
        funcId = GetFuncId(dbg, Address);
        if (funcId)
        {
            if (GetBreakpointLine(rv->address, fileName, &line, TRUE))
            {

                if (GetLocalSymbolAddress(dbg, funcId, name, fileName, line, address, type))
                {
                    *address += rv->basePtr;
                    *dbg_info = dbg;
                    return rv;
                }
            }
        }
        rv = rv->next;
    }
    Address = scope->address - dbg->loadbase + dbg->linkbase;
    if (!GetBreakpointLine(scope->address, fileName, &line, TRUE))
    {
        line = 0;
        fileName[0] = 0;
    }
    if (GetGlobalSymbolAddress(dbg, name, fileName, line, address, type))
    {
        *address += dbg->loadbase - dbg->linkbase;
        *dbg_info = dbg;
        return scope;
    }
    return FALSE;
}

int FindTypeSymbol(DEBUG_INFO** dbg_info, SCOPE* scope, char* name)
{
    DEBUG_INFO* dbg;
    int rv;
    if (!scope)
        return 0;
    dbg = findDebug(scope->address);

    if (!dbg)
        return 0;
    rv = GetType(dbg, name);
    if (rv)
    {
        *dbg_info = dbg;
    }
    return rv;
}

//-------------------------------------------------------------------------

int basictypesize(int type)
{
    if (type >= ePChar && type <= ePULongLong)
        return 4;
    if (type >= ePFloat && type <= ePComplexLongDouble)
        return 4;
    switch (type)
    {
        case eBool:
        case eChar:
        case eUChar:
            return 1;
        case eShort:
        case eUShort:
        case eWcharT:
        case eChar16T:
            return 2;
        case eInt:
        case eUInt:
        case eLong:
        case eULong:
        case eChar32T:
            return 4;
        case eLongLong:
        case eULongLong:
            return 8;
        case eFloat:
        case eImaginary:
            return 4;

        case eDouble:
        case eImaginaryDouble:
            return 8;
        case eLongDouble:
        case eImaginaryLongDouble:
            return 10;
        case eComplex:
            return 8;
        case eComplexDouble:
            return 16;
        case eComplexLongDouble:
            return 20;
        default:
            return 0;
    }
}
void ExpandPointerInfo(DEBUG_INFO* dbg_info, VARINFO* v)
{
    VARINFO* vx = calloc(sizeof(VARINFO), 1);
    if (!vx)
        return;
    v->subtype = vx;
    vx->type = v->type;
    vx->thread = v->thread;
    sprintf(vx->membername, "*%s", v->membername);
    vx->size = DeclType(dbg_info, vx);
}

int GetDbgPointerInfo(DEBUG_INFO* dbg_info, VARINFO* v)
{
    v->pointer = TRUE;
    v->derefaddress = -1;
    v->type = LookupPointerSubtype(dbg_info, v->type);
    ;
    v->subtype = 0;
    return 4;
}
int GetBitFieldInfo(DEBUG_INFO* dbg_info, VARINFO* v)
{
    v->bitfield = TRUE;
    v->type = LookupBitfieldInfo(dbg_info, v->type, &v->bitstart, &v->bitlength);
    return 0;
}
int GetStructInfo(DEBUG_INFO* dbg_info, VARINFO* v, int qual)
{
    // if it is a udt with a name don't override the name
    char dummy[256];
    if (qual == eUnion)
        v->unionx = TRUE;
    else
        v->structure = TRUE;
    v->subtype =
        LookupStructInfo(dbg_info, v->type, v->address, v->udt && v->structtag[0] ? &dummy[0] : &v->structtag[0], &v->arraysize);
    return v->arraysize;
}
int GetArrayInfo(DEBUG_INFO* dbg_info, VARINFO* v)
{
    int i, z;
    VARINFO** nextptr;
    v->array = TRUE;
    v->type = LookupArrayInfo(dbg_info, v->type, &v->arraysize);
    if (v->argument)
    {
        DWORD len;
        /* indirection for arrays specified as parameters */
        ReadProcessMemory(activeProcess->hProcess, (LPVOID)v->address, (LPVOID)&v->address, 4, &len);
    }
    nextptr = &v->subtype;
    i = 0;
    z = 0;
    while (i < v->arraysize)
    {
        VARINFO* vx = calloc(sizeof(VARINFO), 1);
        if (!vx)
            return 0;
        vx->thread = v->thread;
        vx->offset = i;
        vx->address = v->address;
        vx->type = v->type;
        sprintf(vx->membername, "%s[%d]", v->membername, z++);
        i += v->itemsize = vx->size = DeclType(dbg_info, vx);
        *nextptr = vx;
        nextptr = &vx->link;
    }
    return v->arraysize;
}
int GetVLAInfo(DEBUG_INFO* dbg_info, VARINFO* v)
{
    int aa[1024];
    int size = 0;
    int i, z = 0;
    VARINFO** next = &v->subtype;
    v->type = LookupArrayInfo(dbg_info, v->type, &size);
    if (!v->type)
        return 0;
    if (!v->array)
    {
        // top level
        v->array = TRUE;
        v->vararray = TRUE;
        if (!ReadValue(v->address + 4, aa, size - 4, v))
            aa[0] = 0;

        if (aa[0] > 98)
            aa[0] = 0;
        ReadValue(v->address, &v->address, 4, v);

        v->vararraylisttofree = v->vararraylist = calloc((aa[0] + 2), sizeof(int));
        if (v->vararraylist)
            memcpy(v->vararraylist, aa, (aa[0] + 2) * sizeof(int));
        v->arraysize = v->size = aa[aa[0] + 1];
    }
    for (i = 0; i < v->arraysize;)
    {
        VARINFO* vx = calloc(1, sizeof(VARINFO));
        if (!vx)
            return 0;
        vx->thread = v->thread;
        if (v->vararraylevel != v->vararraylist[0] - 1)
        {
            vx->vararraylevel = v->vararraylevel + 1;
            vx->vararraylist = v->vararraylist;
            vx->vararray = TRUE;
            vx->array = TRUE;
            vx->arraysize = vx->size = v->vararraylist[v->vararraylist[0] + 1 - vx->vararraylevel];
        }
        vx->address = v->address + i;
        vx->offset = i;
        vx->type = v->type;
        sprintf(vx->membername, "%s[%d]", v->membername, z++);
        *next = vx;
        next = &(*next)->link;
        i += v->itemsize = DeclType(dbg_info, vx);
    }
    return v->arraysize;
}
int GetEnumInfo(DEBUG_INFO* dbg_info, VARINFO* v)
{
    v->enumx = TRUE;
    LookupEnumValues(dbg_info, v->type, &v->structtag[0]);
    return basictypesize(v->type);
}
int GetTypedefInfo(DEBUG_INFO* dbg_info, VARINFO* v)
{
    if (!v->udt)
    {
        // take the outermost name for nested typedefs
        v->udt = TRUE;
        GetTypeName(dbg_info, v->type, &v->structtag[0]);
    }
    v->type = LookupTypedefValues(dbg_info, v->type);
    return DeclType(dbg_info, v);
}

//-------------------------------------------------------------------------

int DeclType(DEBUG_INFO* dbg_info, VARINFO* v)
{
    int qual;
    if (v->type <= eReservedTop)
    {
        if ((v->type >= ePChar && v->type <= ePULongLong) || v->type == ePVoid)
        {
            v->pointer = TRUE;
            v->derefaddress = -1;
            v->type = v->type - 16;
            v->subtype = NULL;
            return 4;
        }
        if (v->type >= ePFloat && v->type <= ePComplexLongDouble)
        {
            v->pointer = TRUE;
            v->derefaddress = -1;
            v->type = v->type - 24;
            v->subtype = NULL;
            return 4;
        }
        return basictypesize(v->type);
    }
    switch (qual = GetTypeQualifier(dbg_info, v->type))
    {
        case eLRef:
            v->lref = TRUE;
            return GetDbgPointerInfo(dbg_info, v);
        case eRRef:
            v->rref = TRUE;
            return GetDbgPointerInfo(dbg_info, v);
        case ePointer:
        case eFunction:
            return GetDbgPointerInfo(dbg_info, v);
        case eBitField:
            return GetBitFieldInfo(dbg_info, v);
        case eStruct:
        case eUnion:
            return GetStructInfo(dbg_info, v, qual);
        case eArray:
            return GetArrayInfo(dbg_info, v);
        case eVla:
            return GetVLAInfo(dbg_info, v);
        case eEnum:
            return GetEnumInfo(dbg_info, v);
        case eTypeDef:
            return GetTypedefInfo(dbg_info, v);
        case eNone:
        case eField:
            break;
    }
    return 0;
}

//-------------------------------------------------------------------------

VARINFO* GetVarInfo(DEBUG_INFO* dbg, char* name, int address, int type, SCOPE* scope, THREAD* thread)
{
    VARINFO* v = calloc(sizeof(VARINFO), 1);
    if (!v)
        return 0;
    strcpy(v->membername, name);
    v->argument = address - scope->basePtr > 0 && address - scope->basePtr < 1024;
    v->thread = thread;
    v->scope = scope;
    v->address = address;
    v->type = type;
    v->size = DeclType(dbg, v);
    return v;
}

//-------------------------------------------------------------------------

void FreeVarInfo(VARINFO* info)
{
    while (info)
    {
        VARINFO* chain = info->link;
        FreeVarInfo(info->subtype);
        free(info->vararraylisttofree);
        free(info);
        info = chain;
    }
}
