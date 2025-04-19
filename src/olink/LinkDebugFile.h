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

#ifndef LinkDebugFile_h
#define LinkDebugFile_h

#include <map>
#include <string>
#include <vector>
#include "sqlvt.h"
#include <deque>
class ObjFile;
class ObjField;

class LinkDebugFile
{
  public:
    LinkDebugFile(std::string OutputFile, ObjFile* File, std::vector<ObjSection*>& sections,
                  std::map<ObjSection*, ObjSection*>& parentSections) :
        Sections(sections), ParentSections(parentSections), outputFile(std::move(OutputFile)), file(File), dbPointer(nullptr)
    {
    }
    virtual ~LinkDebugFile();
    bool CreateOutput();

  protected:
    bool Begin(void);
    bool End(void);
    bool SQLiteExec(const char* str);
    bool CreateTables(void);
    bool CreateIndexes(void);
    bool DBOpen(char* name);
    bool WriteFileNames();
    bool WriteLineNumbers();
    void PushCPPName(const ObjString& name, int n);
    int GetSQLNameId(const ObjString& name);
    bool WriteNamesTable();
    bool WriteVariableTypes();
    bool WriteVariableNames();
    ObjInt GetSectionBase(ObjExpression* e);
    bool WriteGlobalsTable();
    bool WriteVirtualsTable();
    bool WriteAutosTable();
    bool WriteTypeNamesTable();
    int GetTypeIndex(ObjType* Type)
    {
        if (Type->GetType() < ObjType::eVoid)
            return Type->GetIndex();
        else
            return (int)Type->GetType();
    }

  private:
    struct CPPMapping
    {
        int simpleId;
        int complexId;
    };
    std::string outputFile;
    std::vector<ObjSection*>& Sections;
    ObjFile* file;
    sqlite3* dbPointer;
    std::map<ObjString, int> names;
    std::map<std::string, int> sectionMap;
    std::map<int, int> publicMap, localMap, autoMap, typeMap;
    std::vector<ObjString> nameList;
    std::map<ObjSection*, ObjSection*> ParentSections;
    std::deque<CPPMapping> CPPMappingList;
    static const char* tables;
    static const char* pragmas;
    static const char* indexes;
};
#endif
