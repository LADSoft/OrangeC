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

#include "sqlite3.h"
typedef struct
{
    int argCount;
    char* baseType;
    BOOL member;
    struct _ProtoData
    {
        char* fieldType;
        char* fieldName;
    } * data;
} CCPROTODATA;
typedef struct
{
    int fieldCount;
    int indirectCount;
    struct _structData
    {
        char* fieldType;
        char* fieldName;
        sqlite3_int64 subStructId;
        int indirectCount;
        int flags;
    } * data;
} CCSTRUCTDATA;
typedef struct _ccfuncdata
{
    struct _ccfuncdata* next;
    char* fullname;
    CCPROTODATA* args;
} CCFUNCDATA;
void CodeCompInit(void);
void DoParse(char* name);
void ccLineChange(char* name, int drawnLineno, int delta);
void deleteFileData(char* name);
CCFUNCDATA* ccLookupFunctionList(int lineno, char* file, char* name);
void ccFreeFunctionList(CCFUNCDATA* data);
int ccLookupType(char* buffer, char* name, char* module, int line, int* rflags, sqlite_int64* rtype);
int ccLookupStructType(char* name, char* module, int line, sqlite3_int64* structId, int* indirectCount);
CCSTRUCTDATA* ccLookupStructElems(char* module, sqlite3_int64 structId, int indirectCount);
void ccFreeStructData(CCSTRUCTDATA* data);
int ccLookupContainingNamespace(char* file, int lineno, char* ns);
int ccLookupContainingMemberFunction(char* file, int lineno, char* func);
int ccLookupFunctionType(char* name, char* module, sqlite3_int64* protoId, sqlite3_int64* typeId);
CCPROTODATA* ccLookupArgList(sqlite3_int64 protoId, sqlite3_int64 argId);
void ccFreeArgList(CCPROTODATA* data);
BYTE* ccGetLineData(char* name, int* max);
