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

#ifndef ObjFile_H
#define ObjFile_H

#include <cstring>
#include <vector>
#include <ctime>

#include "ObjTypes.h"
class ObjSection;
class ObjSymbol;
class ObjType;
class ObjBrowseInfo;
class ObjSourceFile;
class ObjFactory;

class ObjFile : public ObjWrapper
{
    typedef std::vector<ObjType*> TypeContainer;
    typedef std::vector<ObjSymbol*> SymbolContainer;
    typedef std::vector<ObjSection*> SectionContainer;
    typedef std::vector<ObjBrowseInfo*> BrowseInfoContainer;
    typedef std::vector<ObjSourceFile*> SourceFileContainer;

  public:
    ObjFile(const ObjString& Name) : name(Name), inputFile(nullptr), bigEndian(false) { memset(&fileTime, 0, sizeof(fileTime)); }
    virtual ~ObjFile() {}
    void Add(ObjSection* Section)
    {
        if (Section)
            sections.push_back(Section);
    }
    void Add(ObjSymbol* Symbol);
    void Add(ObjType* Type)
    {
        if (Type)
            types.push_back(Type);
    }
    void Add(ObjBrowseInfo* BrowseInfo)
    {
        if (BrowseInfo)
            browseInfo.push_back(BrowseInfo);
    }
    void Add(ObjSourceFile* SourceFile)
    {
        if (SourceFile)
            sourceFiles.push_back(SourceFile);
    }
    void ResolveSymbols(ObjFactory* Factory);
    ObjString& GetName() { return name; }
    void SetName(ObjString& Name) { name = Name; }
    ObjSourceFile* GetInputFile() { return inputFile; }
    void SetInputFile(ObjSourceFile* InputFile) { inputFile = InputFile; }
    bool GetBigEndian() { return bigEndian; }
    void SetBigEndian(bool BigEndian) { bigEndian = BigEndian; }

    std::string GetInputName() const { return inputName; }
    void SetInputName(const std::string name) { inputName = std::move(name); }
    std::tm GetFileTime() { return fileTime; }
    void SetFileTime(struct std::tm& FileTime) { fileTime = FileTime; }

    typedef TypeContainer::iterator TypeIterator;
    typedef TypeContainer::const_iterator const_TypeIterator;

    TypeIterator TypeBegin() { return types.begin(); }
    TypeIterator TypeEnd() { return types.end(); }

    typedef SymbolContainer::iterator SymbolIterator;
    typedef SymbolContainer::const_iterator const_SymbolIterator;

    SymbolIterator PublicBegin() { return publics.begin(); }
    SymbolIterator PublicEnd() { return publics.end(); }
    size_t PublicSize() { return publics.size(); }
    SymbolIterator ExternalBegin() { return externals.begin(); }
    SymbolIterator ExternalEnd() { return externals.end(); }
    SymbolIterator LocalBegin() { return locals.begin(); }
    SymbolIterator LocalEnd() { return locals.end(); }
    SymbolIterator AutoBegin() { return autos.begin(); }
    SymbolIterator AutoEnd() { return autos.end(); }
    SymbolIterator RegBegin() { return regs.begin(); }
    SymbolIterator RegEnd() { return regs.end(); }
    SymbolIterator ExportBegin() { return exports.begin(); }
    SymbolIterator ExportEnd() { return exports.end(); }
    size_t ExportSize() { return exports.size(); }
    void ExportClear() { exports.clear(); }
    SymbolIterator ImportBegin() { return imports.begin(); }
    SymbolIterator ImportEnd() { return imports.end(); }
    SymbolIterator DefinitionBegin() { return definitions.begin(); }
    SymbolIterator DefinitionEnd() { return definitions.end(); }

    typedef SectionContainer::iterator SectionIterator;
    typedef SectionContainer::const_iterator const_SectionIterator;

    SectionIterator SectionBegin() { return sections.begin(); }
    SectionIterator SectionEnd() { return sections.end(); }

    typedef BrowseInfoContainer::iterator BrowseInfoIterator;
    typedef BrowseInfoContainer::const_iterator const_BrowseInfoIterator;

    BrowseInfoIterator BrowseInfoBegin() { return browseInfo.begin(); }
    BrowseInfoIterator BrowseInfoEnd() { return browseInfo.end(); }

    typedef SourceFileContainer::iterator SourceFileIterator;
    typedef SourceFileContainer::const_iterator const_SourceFileIterator;

    SourceFileIterator SourceFileBegin() { return sourceFiles.begin(); }
    SourceFileIterator SourceFileEnd() { return sourceFiles.end(); }

    ObjSection* FindSection(const ObjString& Name);
    ObjSection* GetSection(int section) { return sections[section]; }

  private:
    ObjString name;
    ObjSourceFile* inputFile;
    std::tm fileTime;
    bool bigEndian;

    TypeContainer types;
    SymbolContainer publics;
    SymbolContainer externals;
    SymbolContainer locals;
    SymbolContainer autos;
    SymbolContainer regs;
    SymbolContainer imports;
    SymbolContainer exports;
    SymbolContainer definitions;
    SectionContainer sections;
    BrowseInfoContainer browseInfo;
    SourceFileContainer sourceFiles;
    std::string inputName;
};
#endif
