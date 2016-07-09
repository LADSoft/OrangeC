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
#ifndef BRCLoader_H
#define BRCLoader_H

#include "CmdFiles.h"
#include "BRCDictionary.h"
#include <string>
#include <map>
#include <deque>
#include <set>

class ObjFile;
class ObjBrowseInfo;
#include <map>
#include <string>

class SymData;

typedef std::map<std::string, SymData *> Symbols;

class BrowseData
{
public:
    std::string name;	
    std::string hint; // unused
    int func;
    int type;
    int qual;
    int fileIndex;
    int startLine, funcEndLine;
    int blockLevel;
    int line;
    int charPos;
} ;

typedef std::deque<BrowseData *> BrowseDataset;

class SymData
{
public:
    SymData(std::string &Name) : name(Name), externalCount(0), func(NULL),
        globalCount(0), argCount(0), localCount(0), fileOffs(0) { }
    ~SymData();
    std::string name;
    BrowseDataset data;
    BrowseDataset usages;
    std::deque<SymData *> mapping;
    BrowseData *func;
    int externalCount;
    int globalCount;
    int argCount;
    int localCount;
    __int64 index;
    unsigned fileOffs;
    void insert(BrowseData *xx)
    {
        data.push_back(xx);
    }
} ;
class BlockData
{
public:
    BlockData() : start(0), end(0), max(0), count(0) { }
    ~BlockData();
    std::map<std::string, BrowseData *> syms;
    int start;
    int end;
    int max;
    int count;
};
class BRCLoader
{
public:
    BRCLoader(CmdFiles &Files ) : files(Files), currentFile(0), blockHead(0) { }
    ~BRCLoader();
    bool load();

    Symbols &GetSymbols() { return syms; }
    std::map<std::string, int> &GetFileNames() { return nameMap; }    
protected:
    void InsertSymData(std::string s, BrowseData *data, bool func = false);
    void InsertBlockData(std::string s, BrowseData *ldata);
    void InsertFile(ObjBrowseInfo &p);
    int InsertVariable(ObjBrowseInfo &p, bool func = false);
    void InsertDefine(ObjBrowseInfo &p);
    void StartFunc(ObjBrowseInfo &p);
    void EndFunc(ObjBrowseInfo &p);
    void StartBlock(int line);
    void EndBlock(int line);
    void Usages(ObjBrowseInfo &p);
    void ParseData(ObjFile &f);
    void LoadSourceFiles(ObjFile &fil);
private:
    int currentFile;
    CmdFiles &files;
    std::map<std::string, int> nameMap;
    std::map<int, int> indexMap;	
    Symbols syms;
    std::vector<BlockData *> blocks;
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