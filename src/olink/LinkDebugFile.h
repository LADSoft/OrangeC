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
#ifndef LinkDebugFile_h
#define LinkDebugFile_h

#include <map>
#include <string>
#include <vector>
#include "sqlvt.h"
class ObjFile;
class ObjField;

class LinkDebugFile
{
public:
    LinkDebugFile(std::string OutputFile, ObjFile *File) : outputFile(OutputFile), file(File), dbPointer(NULL) { }
    virtual ~LinkDebugFile();
    bool CreateOutput();
protected:
    bool Begin(void);
    bool End(void );
    bool SQLiteExec( char *str);
	bool CreateTables(void);
    bool CreateIndexes(void);
    bool DBOpen(char *name);
    bool WriteFileNames();
    bool WriteLineNumbers();
    int GetSQLNameId(ObjString name);
    bool WriteNamesTable();
    bool WriteVariableTypes();
    bool WriteVariableNames();
    ObjInt GetSectionBase(ObjExpression *e);
    bool WriteGlobalsTable();
    bool WriteAutosTable();
    bool WriteTypeNamesTable();
    int  GetTypeIndex(ObjType *Type)
    {
        if (Type->GetType() < ObjType::eVoid)
            return Type->GetIndex();
        else
            return (int)Type->GetType();
    }
private:
    std::string outputFile;
    ObjFile *file;
    sqlite3 *dbPointer;
    std::map<ObjString, int> names;
    std::map<int, int> publicMap, localMap, autoMap, typeMap;
    std::vector<ObjString *>nameList;
    static char *tables;
    static char *pragmas;
    static char *indexes;
};
#endif
