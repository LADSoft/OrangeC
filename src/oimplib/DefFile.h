/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#ifndef DefFile_h
#define DefFile_h

#include "Token.h"

#include <string>
#include <fstream>
#include <map>
#include <deque>
#include <memory>

enum class kw
{
    none = 256,
    at,
    dot,
    equals,
    name,
    library,
    exports,
    imports,
    description,
    stacksize,
    heapsize,
    code,
    data,
    sections,
    noname,
    constant,
    private_,
    read,
    write,
    execute,
    shared
};

class DefFile
{
  public:
    DefFile(const std::string& fname) :
        fileName(fname), tokenizer("", &keywords), lineno(0), imageBase(-1), stackSize(-1), heapSize(-1), token(nullptr)
    {
        Init();
    }
    virtual ~DefFile();
    bool Read();
    bool Write();
    std::string GetName() { return fileName; }
    void SetFileName(const std::string& fname) { fileName = fname; }
    std::string GetLibraryName() { return library; }
    void SetLibraryName(const std::string& name) { library = name; }
    struct Export
    {
        Export() : byOrd(false), ord(-1) {}
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
    void Add(Export* e);
    void Add(Import* i);
    typedef std::deque<std::unique_ptr<Export>>::iterator ExportIterator;
    ExportIterator ExportBegin() { return exports.begin(); }
    ExportIterator ExportEnd() { return exports.end(); }
    typedef std::deque<std::unique_ptr<Import>>::iterator ImportIterator;
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
    void ReadSectionsInternal(const char* name);
    void ReadCode();
    void ReadData();
    void ReadSections();

    void WriteName();
    void WriteLibrary();
    void WriteExportLine(const char *modifiedId, Export* exp);
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
    const Token* token;
    std::fstream stream;
    int lineno;
    std::map<std::string, unsigned> sectionMap;
    std::deque<std::unique_ptr<Export>> exports;
    std::deque<std::unique_ptr<Import>> imports;
    static bool initted;
    static KeywordHash keywords;
};
#endif