/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

#ifndef BRCLoader_H
#define BRCLoader_H

#include "CmdFiles.h"
#include "BRCDictionary.h"
#include <string>
#include <map>
#include <deque>
#include <set>
#include <memory>

class ObjFile;
class ObjBrowseInfo;

class SymData;

typedef std::map<std::string, std::unique_ptr<SymData>> Symbols;

class BrowseData
{
  public:
    std::string name;
    std::string hint;  // unused
    int func;
    int type;
    int qual;
    int fileIndex;
    int startLine, funcEndLine;
    int blockLevel;
    int line;
    int charPos;
};

typedef std::deque<std::unique_ptr<BrowseData>> BrowseDataset;

class SymData
{
  public:
    SymData(std::string& Name);
    ~SymData();
    std::string name;
    BrowseDataset data;
    BrowseDataset usages;
    std::deque<SymData*> mapping;
    BrowseData* func;
    int externalCount;
    int globalCount;
    int argCount;
    int localCount;
    long long index;
    unsigned fileOffs;
    void insert(std::unique_ptr<BrowseData> xx) { data.push_back(std::move(xx)); }
};
class BlockData
{
  public:
    BlockData() : start(0), end(0), max(0), count(0) {}
    ~BlockData();
    std::map<std::string, BrowseData*> syms;
    int start;
    int end;
    int max;
    int count;
};
class BRCLoader
{
  public:
    BRCLoader(CmdFiles& Files) : files(Files), currentFile(0), blockHead(0) {}
    ~BRCLoader();
    bool load();

    Symbols& GetSymbols() { return syms; }
    std::map<std::string, int>& GetFileNames() { return nameMap; }

  protected:
    void InsertSymData(std::string s, BrowseData* data, bool func = false);
    void InsertBlockData(std::string s, BrowseData* ldata);
    void InsertFile(ObjBrowseInfo& p);
    int InsertVariable(ObjBrowseInfo& p, bool func = false);
    void InsertDefine(ObjBrowseInfo& p);
    void StartFunc(ObjBrowseInfo& p);
    void EndFunc(ObjBrowseInfo& p);
    void StartBlock(int line);
    void EndBlock(int line);
    void Usages(ObjBrowseInfo& p);
    void ParseData(ObjFile& f);
    void LoadSourceFiles(ObjFile& fil);

  private:
    int currentFile;
    CmdFiles& files;
    std::map<std::string, int> nameMap;
    std::map<int, int> indexMap;
    Symbols syms;
    std::vector<BlockData*> blocks;
    int blockHead;
    std::deque<int> functionNesting;
};
#endif

#if 0

    500:  browse information record.
        CO500,subtype,qualifiers,file index,linenumber,name.

        The browse info record depends on the subtype.
        Valid subtypes are:

        0: This is for preprocessor definitions like #defines.
        1: variable:  this is some type of program variable.
        2: file start: this is a file start
        3: function start: this is a function start
        4: function end: this is the end of a function
        5: block start: this is the start of a block
        6: block end: this is the end of a block

        qualifiers are used for variable and function names:

        0: program global
        1: file global
        2: external
        3: local variable
        4: prototype
        5: type

        the file index references a CO300 record.
        the line number references the line within the given file.

#endif