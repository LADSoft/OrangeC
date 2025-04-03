/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 *
 */

#ifndef DB_H
#define DB_H
#include "../sqlite3/sqlite3.h"

namespace CompletionCompiler
{
void ccReset(void);
int ccDBOpen(const char* name);
int ccBegin(void);
int ccEnd(void);
int ccDBDeleteForFile(sqlite3_int64 id);
int ccWriteName(const char* name, sqlite_int64* id, sqlite_int64 main_id);
int ccWriteStructName(const char* name, sqlite_int64* id);
int ccWriteFileName(const char* name, sqlite_int64* id);
int ccWriteFileTime(const char* name, int time, sqlite_int64* id);
int ccWriteLineNumbers(const char* symname, const char* nameoftype, const char* filename, int indirectCount, sqlite_int64 struct_id,
                       sqlite3_int64 main_id, int start, int end, int altend, int flags, sqlite_int64* id);
int ccWriteLineData(sqlite_int64 file_id, sqlite_int64 main_id, const char* data, int len, int lines);
int ccWriteGlobalArg(sqlite_int64 line_id, sqlite_int64 main_id, const char* symname, const char* nameoftype, int* order);
int ccWriteStructField(sqlite3_int64 name_id, const char* symname, const char* nameoftype, int indirectCount,
                       sqlite_int64 struct_id, sqlite3_int64 file_id, sqlite3_int64 main_id, int flags, int* order,
                       sqlite_int64* id);
int ccWriteMethodArg(sqlite_int64 struct_id, const char* nameoftype, int* order, sqlite3_int64 main_id);
int ccWriteSymbolType(const char* symname, sqlite3_int64 file_id, const char* declFile, int startLine, int endLine, int type);
int ccWriteNameSpaceEntry(const char* symname, sqlite_int64 file_id, const char* filename, int startline, int endline);
int ccWriteUsingRecord(const char* symname, const char* parentname, const char* file, int line, sqlite_int64 main_id);
}  // namespace CompletionCompiler
#endif  // DB_H