/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
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
#ifndef BRCWriter_H
#define BRCWriter_H

#include <string>
#include <fstream>
#include "sqlite3.h"

class BRCLoader;

class BRCWriter
{
public:
    BRCWriter(std::string OutputFile, BRCLoader &Loader) : outputFile(OutputFile), loader(Loader) { }
    ~BRCWriter();

    bool write();

protected:
    bool Begin(void);
    bool End(void );
    static int NullCallback(void *NotUsed, int argc, char **argv, char **azColName);
    static int VersionCallback(void *NotUsed, int argc, char **argv, char **azColName);
    int SQLiteExec( char *str);
    int CheckDb(void);
    int CreateTables(void);
    int DBOpen(char *name);

    bool Insert(sqlite3_int64 simpleId, sqlite3_int64 complexId);
    bool Insert(std::string fileName, int index);
    bool Insert(std::string symName, int type, sqlite3_int64 *id);
    bool Insert(sqlite3_int64 fileIndex, int start, int end, sqlite3_int64 nameIndex);
    bool Insert(sqlite3_int64 symIndex, BrowseData *b, bool usages = false);
    
    bool WriteFileList();
    void InsertMappingSym(std::string name, SymData *orig, std::map<std::string, SymData *> &syms,
                     std::map<std::string, SymData *> &newSyms);
    void PushCPPNames(std::string name, SymData * orig, std::map<std::string, SymData *> &syms, 
                     std::map<std::string, SymData *> &newSyms);
    bool WriteDictionary(Symbols &syms);
    bool WriteMapping(Symbols &syms);
    bool WriteLineData(Symbols &syms);
    bool WriteUsageData(Symbols &syms);
    bool WriteJumpTable(Symbols &syms);       

    BRCLoader &loader;
private:
    std::string outputFile;
    int fileListCount;
    sqlite3 *dbPointer;
    std::map<int, sqlite3_int64> fileMap;
    
    static char *tables;
    static char *deletion;
} ;
#endif
