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

#ifndef LINKREGION_H
#define LINKREGION_H

#include <cstring>
#include <vector>
#include <vector>
#include <map>
#include <set>
#include "LinkAttribs.h"

class LinkOverlay;
class ObjFile;
class ObjSection;
class LinkExpression;
class LinkTokenizer;
class CmdFiles;
class LinkRegionFileSpecContainer;
class LinkManager;

class LinkRegion
{

  public:
    struct OneSection
    {
        OneSection() : file(nullptr), section(nullptr) {}
        OneSection(ObjFile* File, ObjSection* Section) : file(File), section(Section) {}
        ObjFile* file;
        ObjSection* section;
    };
    typedef std::vector<OneSection>::iterator OneSectionIterator;
    struct NamedSection
    {
        NamedSection(std::string Name) : name(Name) {}
        std::string name;
        std::vector<OneSection> sections;
    };
    struct nslt
    {
        bool operator()(const NamedSection* left, const NamedSection* right) const
        {
            return strcmp(left->name.c_str(), right->name.c_str()) < 0;
        }
    };
    typedef std::vector<ObjString> SourceFile;
    typedef std::vector<std::unique_ptr<NamedSection>> SectionData;
    LinkRegion(LinkOverlay* Parent) : name(""), common(false), maxSize(-1), parent(Parent), overlayed(false) {}
    ~LinkRegion();

    ObjString& GetName() { return name; }
    void SetName(const ObjString& Name) { name = Name; }
    ObjString QualifiedRegionName();

    LinkOverlay* GetParent() { return parent; }
    void SetParent(LinkOverlay* Parent) { parent = Parent; }

    void SetAttribs(const LinkAttribs& Attribs) { attribs = Attribs; }
    LinkAttribs& GetAttribs() { return attribs; }

    typedef SourceFile::iterator SourceFileIterator;

    SourceFileIterator SourceFileBegin() { return sourceFiles.begin(); }
    SourceFileIterator SourceFileEnd() { return sourceFiles.end(); }

    typedef SectionData::iterator SectionDataIterator;
    typedef std::set<NamedSection*, nslt> LookasideBuf;

    SectionDataIterator NowDataBegin() { return nowData.begin(); }
    SectionDataIterator NowDataEnd() { return nowData.end(); }
    SectionDataIterator NormalDataBegin() { return normalData.begin(); }
    SectionDataIterator NormalDataEnd() { return normalData.end(); }
    SectionDataIterator PostponeDataBegin() { return postponeData.begin(); }
    SectionDataIterator PostponeDataEnd() { return postponeData.end(); }

    void AddSourceFile(CmdFiles& files, const ObjString& spec);
    void AddSection(LinkManager* manager);

    bool ParseRegionSpec(LinkManager* manager, CmdFiles& files, LinkTokenizer& spec);

    ObjInt PlaceRegion(LinkManager* manager, LinkAttribs& partitionAttribs, ObjInt base);

    void AddNowData(ObjFile* file, ObjSection* section) { AddData(nowData, nowLookaside, file, section); }
    void AddNormalData(ObjFile* file, ObjSection* section) { AddData(normalData, normalLookaside, file, section); }
    void AddPostponeData(ObjFile* file, ObjSection* section) { AddData(postponeData, postponeLookaside, file, section); }

  protected:
    void AddData(SectionData& data, LookasideBuf& lookaside, ObjFile* file, ObjSection* section);

  private:
    std::map<std::string, int> equalSections;
    ObjInt ArrangeOverlayed(LinkManager* manager, NamedSection* data, ObjInt address);
    void AddFile(ObjFile* file);
    bool ParseFiles(CmdFiles& files, LinkTokenizer& spec);
    bool ParseName(LinkTokenizer& spec);
    bool ParseAttributes(LinkTokenizer& spec);
    bool ParseValue(LinkTokenizer& spec, LinkExpression** rv);
    bool HoldsFile(const ObjString& fileName);
    void CheckAttributes();
    bool Matches(const ObjString& name, const ObjString& spec);
    LinkRegionFileSpecContainer& Split(ObjString& spec);
    ObjInt ArrangeSections(LinkManager* manager);
    ObjString name;
    bool common;
    bool overlayed;
    SourceFile sourceFiles;
    SectionData nowData;
    SectionData normalData;
    SectionData postponeData;
    LookasideBuf nowLookaside;
    LookasideBuf normalLookaside;
    LookasideBuf postponeLookaside;
    LinkAttribs attribs;
    LinkOverlay* parent;
    ObjInt maxSize;
};
#endif
