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

#ifndef BRCWriter_H
#define BRCWriter_H

#include <string>
#include <fstream>
#include "sqlite3.h"

class BRCLoader;

class BRCWriter
{
  public:
    BRCWriter(std::string OutputFile, BRCLoader& Loader) :
        outputFile(OutputFile), loader(Loader), fileListCount(0), dbPointer(nullptr)
    {
    }
    ~BRCWriter();

    bool write();

  protected:
    bool Begin(void);
    bool End(void);
    static int NullCallback(void* NotUsed, int argc, char** argv, char** azColName);
    static int VersionCallback(void* NotUsed, int argc, char** argv, char** azColName);
    int SQLiteExec(const char* str);
    int CheckDb(void);
    int CreateTables(void);
    int DBOpen(char* name);

    bool Insert(sqlite3_int64 simpleId, sqlite3_int64 complexId);
    bool Insert(std::string fileName, int index);
    bool Insert(std::string symName, int type, sqlite3_int64* id);
    bool Insert(sqlite3_int64 fileIndex, int start, int end, sqlite3_int64 nameIndex);
    bool Insert(sqlite3_int64 symIndex, BrowseData* b, bool usages = false);

    bool WriteFileList();
    void InsertMappingSym(std::string name, SymData* orig, Symbols& syms, Symbols& newSyms);
    void PushCPPNames(std::string name, SymData* orig, Symbols& syms, Symbols& newSyms);
    bool WriteDictionary(Symbols& syms);
    bool WriteMapping(Symbols& syms);
    bool WriteLineData(Symbols& syms);
    bool WriteUsageData(Symbols& syms);
    bool WriteJumpTable(Symbols& syms);

    BRCLoader& loader;

  private:
    std::string outputFile;
    int fileListCount;
    sqlite3* dbPointer;
    std::map<int, sqlite3_int64> fileMap;

    static const char* tables;
    static const char* deletion;
};
#endif
