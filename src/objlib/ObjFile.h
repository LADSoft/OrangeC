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
#ifndef ObjFile_H
#define ObjFile_H

#include <string.h>
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
    typedef std::vector<ObjType *> TypeContainer;
    typedef std::vector<ObjSymbol *> SymbolContainer;
    typedef std::vector<ObjSection *> SectionContainer;
    typedef std::vector<ObjBrowseInfo *> BrowseInfoContainer;
    typedef std::vector<ObjSourceFile *> SourceFileContainer;
public:
    ObjFile (ObjString Name) : name(Name), inputFile(nullptr), bigEndian(false)
    { 
        memset(&fileTime, 0, sizeof(fileTime));
    }
    virtual ~ObjFile () {}
    void Add(ObjSection *Section)
    {
        if (Section)
            sections.push_back(Section);
    }
    void Add(ObjSymbol *Symbol);
    void Add(ObjType *Type)
    {
        if (Type)
            types.push_back(Type);
    }
    void Add(ObjBrowseInfo *BrowseInfo)
    {
        if (BrowseInfo)
            browseInfo.push_back(BrowseInfo);
    }
    void Add(ObjSourceFile *SourceFile)
    {
        if (SourceFile)
            sourceFiles.push_back(SourceFile);
    }
    void ResolveSymbols(ObjFactory *Factory);
    ObjString &GetName() { return name; }
    void SetName(ObjString Name) { name = Name; }
    ObjSourceFile *GetInputFile() { return inputFile; }
    void SetInputFile(ObjSourceFile *InputFile) { inputFile = InputFile; }
    bool GetBigEndian() { return bigEndian; }
    void SetBigEndian(bool BigEndian) { bigEndian = BigEndian; }
    
    std::string GetInputName() const { return inputName; }
    void SetInputName(const std::string name) { inputName = name; }
    std::tm GetFileTime() { return fileTime; }
    void SetFileTime(struct std::tm &FileTime) { fileTime = FileTime; }

    typedef TypeContainer::iterator TypeIterator;
    typedef TypeContainer::const_iterator const_TypeIterator;

    TypeIterator TypeBegin() { return types.begin(); }
    TypeIterator TypeEnd() { return types.end(); }
    
    typedef SymbolContainer::iterator SymbolIterator;
    typedef SymbolContainer::const_iterator const_SymbolIterator;

    SymbolIterator PublicBegin() { return publics.begin(); }
    SymbolIterator PublicEnd() { return publics.end(); }
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

    ObjSection *FindSection(const ObjString Name);
    ObjSection *GetSection(int section) { return sections[section]; }
    
private:
    ObjString name;
    ObjSourceFile *inputFile;
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
} ;
#endif
