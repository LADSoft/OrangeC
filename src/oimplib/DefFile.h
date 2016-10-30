/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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
#ifndef DefFile_h
#define DefFile_h

#include "Token.h"

#include <string>
#include <fstream>
#include <map>
#include <deque>

enum {
    edt_none = 256,
    edt_at,
    edt_dot,
    edt_equals,
    edt_name,
    edt_library,
    edt_exports,
    edt_imports,
    edt_description,
    edt_stacksize,
    edt_heapsize,
    edt_code,
    edt_data,
    edt_sections,
    edt_noname,
    edt_constant,
    edt_private,
    edt_read,
    edt_write,
    edt_execute,
    edt_shared
};

class DefFile
{
public:
    DefFile(const std::string &fname) : fileName(fname), tokenizer("", &keywords), lineno(0), imageBase(-1),
        stackSize(-1), heapSize(-1), token(nullptr)
        { Init(); }
    virtual ~DefFile();
    bool Read();
    bool Write();
    std::string GetName() { return fileName; }
    void SetFileName(const std::string &fname) { fileName = fname; }
    std::string GetLibraryName() { return library; }
    void SetLibraryName(const std::string &name) { library = name; }
    struct Export
    {
        Export() : byOrd(false), ord(-1) { }
        bool byOrd;
        unsigned ord;
        std::string id;
        std::string module;
        std::string entry;
    };
    struct Import
    {
        std::string id;
        std::string module;
        std::string entry;
    };
    void Add(Export *e) { exports.push_back(e); }
    void Add(Import *i) { imports.push_back(i); }
    typedef std::deque<Export *>::iterator ExportIterator;
    ExportIterator ExportBegin() { return exports.begin(); }
    ExportIterator ExportEnd() { return exports.end(); }
    typedef std::deque<Import *>::iterator ImportIterator;
    ImportIterator ImportBegin() { return imports.begin(); }
    ImportIterator ImportEnd() { return imports.end(); }
protected:
    void Init();
    void NextToken();
    
    void ReadName();
    void ReadLibrary();
    void ReadExports();
    void ReadImports();
    void ReadDescription();
    void ReadStacksize();
    void ReadHeapsize();
    void ReadSectionsInternal(const char *name);
    void ReadCode();
    void ReadData();
    void ReadSections();

    void WriteName();
    void WriteLibrary();
    void WriteExports();
    void WriteImports();
    void WriteDescription();
    void WriteStacksize();
    void WriteHeapsize();
    void WriteSectionBits(unsigned value);
    void WriteCode();
    void WriteData();
    void WriteSections();

private:
    std::string fileName;
    std::string name;
    std::string library;
    std::string description;
    unsigned imageBase;
    unsigned stackSize;
    unsigned heapSize;
    Tokenizer tokenizer;
    const Token *token;
    std::fstream *stream;
    int lineno;
    std::map<std::string, unsigned> sectionMap;
    std::deque<Export *> exports;
    std::deque<Import *> imports;
    static bool initted;
    static KeywordHash keywords;
} ;
#endif