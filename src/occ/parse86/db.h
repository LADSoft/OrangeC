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

#ifndef DB_H
#define DB_H
#include "../../sqlite3/sqlite3.h"

void ccReset(void);
int ccDBOpen(char *name);
int ccBegin(void );
int ccEnd(void );
int ccDBDeleteForFile( sqlite3_int64 id);
int ccWriteName( char *name, sqlite_int64 *id, sqlite_int64 main_id);
int ccWriteStructName( char *name, sqlite_int64 *id);
int ccWriteFileName( char *name, sqlite_int64 *id);
int ccWriteFileTime( char *name, int time, sqlite_int64 *id);
int ccWriteLineNumbers( char *symname, char *nameoftype, char *filename, 
                       int indirectCount, sqlite_int64 struct_id, 
                       sqlite3_int64 main_id, int start, int end, int altend,
                       int flags, sqlite_int64 *id);
int ccWriteLineData(sqlite_int64 file_id, sqlite_int64 main_id, char *data, int len, int lines);
int ccWriteGlobalArg( sqlite_int64 line_id, sqlite_int64 main_id, char *symname, char *nameoftype, int *order);
int ccWriteStructField( sqlite3_int64 name_id, char *symname, char *nameoftype, 
                       int indirectCount, sqlite_int64 struct_id, 
                       sqlite3_int64 file_id, sqlite3_int64 main_id, 
                       int flags, int *order, sqlite_int64 *id);
int ccWriteMethodArg( sqlite_int64 struct_id, char *nameoftype, int *order, sqlite3_int64 main_id);
int ccWriteSymbolType( char *symname, sqlite3_int64 file_id, char *declFile, int startLine, int endLine, int type);
int ccWriteNameSpaceEntry(char *symname, sqlite_int64 file_id, char *filename, int startline, int endline);
int ccWriteUsingRecord(char *symname, char *parentname, char *file, int line, sqlite_int64 main_id);
#endif //DB_H