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

#include "LinkRegion.h"
#include "LinkRegionFileSpec.h"
#include "LinkOverlay.h"
#include "LinkPartition.h"
#include "LinkManager.h"
#include "LinkTokenizer.h"
#include "LinkExpression.h"
#include "CmdFiles.h"
#include "LinkManager.h"
#include "LinkNameLogic.h"
#include "ObjSection.h"
#include "ObjFile.h"
#include "UTF8.h"
//#include <dir.h>
#include <cctype>

LinkRegion::~LinkRegion() {}
ObjString LinkRegion::QualifiedRegionName()
{
    ObjString partName(GetParent()->GetParent()->GetName());
    ObjString overlayName(GetParent()->GetName());
    if (name != partName)
    {
        return partName + "::" + overlayName + "::" + name;
    }
    else if (name != overlayName)
    {
        return overlayName + "::" + name;
    }
    else
        return name;
}
bool LinkRegion::ParseFiles(CmdFiles& files, LinkTokenizer& spec)
{

    if (!spec.Matches(LinkTokenizer::eBegin))
        return false;
    bool done = false;
    const char* p = spec.GetData();
    const char* s = p;
    while (!done)
    {
        while (isspace(*s))
            s++;
        char buf[256], *q = buf;
        bool quote = false;
        if (*s == '"')
        {
            s++;
            while (*s && *s != '}' && *s != '"')
                *q++ = *s++;
        }
        else
        {
            while (*s && *s != '}' && !isspace(*s))
                *q++ = *s++;
        }
        *q = 0;
        if (!buf[0])
            break;
        AddSourceFile(files, buf);
        if (*s == '"')
            s++;
        while (isspace(*s))
            s++;
        if (*s != ',')
            done = true;
    }
    spec.Skip(s - p);
    // not proceeding to next token here
    return spec.Matches(LinkTokenizer::eEnd);
}
bool LinkRegion::ParseName(LinkTokenizer& spec)
{
    char buf[4096], *q = buf;
    const char* p = spec.GetData();
    while (isspace(*p) || UTF8::IsAlnum(p) || *p == '*' || *p == '?' || *p == '.' || *p == '|' || *p == '&' || *p == '!' ||
           *p == '(' || *p == ')')
    {
        int v = UTF8::CharSpan(p);
        for (int i = 0; i < v && *p; i++)
            *q++ = *p++;
    }
    *q = 0;
    spec.Skip(q - buf);
    SetName(buf);
    return true;
}
bool LinkRegion::ParseValue(LinkTokenizer& spec, LinkExpression** rv)
{
    spec.NextToken();
    if (!spec.MustMatch(LinkTokenizer::eAssign))
        return false;
    if (!spec.GetExpression(rv, false))
        return false;
    return true;
}
bool LinkRegion::ParseAttributes(LinkTokenizer& spec)
{
    /* optional */
    if (spec.Matches(LinkTokenizer::eBracketOpen))
    {
        bool done = false;
        spec.NextToken();
        while (!done)
        {
            switch (spec.GetTokenType())
            {
                LinkExpression* value;
                case LinkTokenizer::eAddr:
                    if (!ParseValue(spec, &value))
                        return false;
                    attribs.SetAddress(value);
                    break;
                case LinkTokenizer::eSize:
                    if (!ParseValue(spec, &value))
                        return false;
                    attribs.SetSize(value);
                    break;
                case LinkTokenizer::eMaxSize:
                    if (!ParseValue(spec, &value))
                        return false;
                    attribs.SetMaxSize(value);
                    break;
                case LinkTokenizer::eRoundSize:
                    if (!ParseValue(spec, &value))
                        return false;
                    attribs.SetRoundSize(value);
                    break;
                case LinkTokenizer::eVirtual:
                    if (!ParseValue(spec, &value))
                        return false;
                    attribs.SetVirtualOffset(value);
                    break;
                case LinkTokenizer::eFill:
                    if (!ParseValue(spec, &value))
                        return false;
                    attribs.SetFill(value);
                    break;
                case LinkTokenizer::eAlign:
                    if (!ParseValue(spec, &value))
                        return false;
                    attribs.SetAlign(value);
                    break;
                default:
                    return false;
            }
            if (spec.GetTokenType() != LinkTokenizer::eComma)
                done = true;
            else
                spec.NextToken();
        }
        return spec.MustMatch(LinkTokenizer::eBracketClose);
    }
    return true;
}
bool LinkRegion::ParseRegionSpec(LinkManager* manager, CmdFiles& files, LinkTokenizer& spec)
{
    if (!ParseFiles(files, spec))
        return false;
    if (!ParseName(spec))
        return false;
    if (!ParseAttributes(spec))
        return false;
    AddSection(manager);
    return true;
}
bool LinkRegion::HoldsFile(const ObjString& fileName)
{
    for (auto name : sourceFiles)
    {
        if (name == fileName)
            return true;
    }
    return false;
}
bool LinkRegion::Matches(const ObjString& name, const ObjString& spec)
{
    LinkRegionFileSpecContainer fs(spec);
    return fs.Matches(name);
}
void LinkRegion::AddSourceFile(CmdFiles& filelist, const ObjString& spec)
{
    for (auto&& name : filelist)
        if (Matches(name, spec))
            sourceFiles.push_back(name);
}
void LinkRegion::AddData(SectionData& data, LookasideBuf& lookaside, ObjFile* file, ObjSection* section)
{
    NamedSection* ns = nullptr;
    NamedSection aa(section->GetName());
    auto it = lookaside.find(&aa);
    if (it != lookaside.end())
        ns = *it;
    if (ns == nullptr)
    {
        data.push_back(std::make_unique<NamedSection>(section->GetName()));
        ns = data.back().get();
        ns->name = section->GetName();
        lookaside.insert(data.back().get());
    }
    ns->sections.push_back(OneSection(file, section));
}

void LinkRegion::AddFile(ObjFile* file)
{
    LinkNameLogic logic(name);
    for (auto it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        ObjSection* sect = *it;
        if (logic.Matches(sect->GetName()))
        {
            ObjInt quals = sect->GetQuals();
            if (attribs.GetAlign() < sect->GetAlignment())
                attribs.SetAlign(new LinkExpression(sect->GetAlignment()));
            // using section::utility flag to for placed regions
            sect->SetUtilityFlag(true);
            if (quals & ObjSection::now)
            {
                if (quals & ObjSection::postpone)
                    LinkManager::LinkError("file " + file->GetName() + "Region " + QualifiedRegionName() +
                                           " has both 'now' and 'postpone' qualifiers");
                AddNowData(file, sect);
            }
            else if (quals & ObjSection::postpone)
            {
                AddPostponeData(file, sect);
            }
            else
            {
                AddNormalData(file, sect);
            }
        }
    }
}
void LinkRegion::AddSection(LinkManager* manager)
{
    for (auto it = manager->FileBegin(); it != manager->FileEnd(); ++it)
    {
        ObjFile* file = *it;
        if (sourceFiles.empty())
            AddFile(file);
        else
            for (auto srcFile : sourceFiles)
            {
                if (srcFile == file->GetInputName())
                {
                    AddFile(file);
                    break;
                }
            }
    }
}
#define max(x, y) (((x) > (y)) ? (x) : (y))
void LinkRegion::CheckAttributes()
{
    int n = nowData.size() + postponeData.size() + normalData.size();
    if (n)
    {
        bool anyCommon = false;
        bool anyNonCommon = false;
        bool anyUnique = false;
        bool anyEqual = false;
        bool anyMax = false;
        bool anySeparate = false;
        bool notEqual = false;
        bool used = false;
        ObjInt maxAlign = -1;
        for (auto& data : nowData)
        {
            for (auto item : data->sections)
            {
                ObjSection* sect = item.section;
                ObjInt quals = sect->GetQuals();
                if (maxSize != -1 && maxSize != sect->GetAbsSize())
                    notEqual = true;
                maxSize = max(maxSize, sect->GetAbsSize());
                maxAlign = max(maxAlign, sect->GetAlignment());
                if (quals & ObjSection::equal)
                {
                    anyEqual = true;
                    anyCommon = true;
                }
                else if (quals & ObjSection::max)
                {
                    anyMax = true;
                    anyCommon = true;
                }
                else if (quals & ObjSection::common)
                    anyCommon = true;
                else
                    anyNonCommon = true;
                if (quals & ObjSection::unique)
                    anyUnique = true;
                if (quals & ObjSection::separate)
                    anySeparate = true;
            }
        }
        for (auto& data : normalData)
        {
            for (auto item : data->sections)
            {
                ObjSection* sect = item.section;
                ObjInt quals = sect->GetQuals();
                if (maxSize != -1 && maxSize != sect->GetAbsSize())
                    notEqual = true;
                maxSize = max(maxSize, sect->GetAbsSize());
                maxAlign = max(maxAlign, sect->GetAlignment());
                if (quals & ObjSection::equal)
                {
                    anyEqual = true;
                    anyCommon = true;
                }
                else if (quals & ObjSection::max)
                {
                    anyMax = true;
                    anyCommon = true;
                }
                else if (quals & ObjSection::common)
                    anyCommon = true;
                else
                    anyNonCommon = true;
                if (quals & ObjSection::unique)
                    anyUnique = true;
                if (quals & ObjSection::separate)
                    anySeparate = true;
            }
        }
        for (auto& data : postponeData)
        {
            for (auto item : data->sections)
            {
                ObjSection* sect = item.section;
                ObjInt quals = sect->GetQuals();
                if (maxSize != -1 && maxSize != sect->GetAbsSize())
                    notEqual = true;
                maxSize = max(maxSize, sect->GetAbsSize());
                maxAlign = max(maxAlign, sect->GetAlignment());
                if (quals & ObjSection::equal)
                {
                    anyEqual = true;
                }
                else if (quals & ObjSection::max)
                {
                    anyMax = true;
                }
                else if (quals & ObjSection::common)
                    anyCommon = true;
                else
                    anyNonCommon = true;
                if (quals & ObjSection::unique)
                    anyUnique = true;
                if (quals & ObjSection::separate)
                    anySeparate = true;
            }
        }
        overlayed = anyMax | anyEqual;
        if (anyCommon && !anyEqual && !anyMax)
            common = true;
        if (n > 1 && anyUnique)
            LinkManager::LinkError("Region " + QualifiedRegionName() + " unique qualifier with multiple sections");
        if ((anyCommon && anyNonCommon) || (anyEqual && anyNonCommon) || (anyMax && anyNonCommon))
            LinkManager::LinkError("Region " + QualifiedRegionName() + " Mixing Common and NonCommon sections");
        if (anyEqual && anyMax)
            LinkManager::LinkError("Region " + QualifiedRegionName() + " Mixing equal and max characteristics");
        if (anyEqual && notEqual)
            LinkManager::LinkError("Region " + QualifiedRegionName() + " Equal sections with different size");
        if (anySeparate && anyCommon)
            LinkManager::LinkError("Region " + QualifiedRegionName() + " Mixing separate and common sections");
        if (maxAlign > attribs.GetAlign())
            attribs.SetAlign(new LinkExpression(maxAlign));
    }
}
ObjInt LinkRegion::ArrangeOverlayed(LinkManager* manager, NamedSection* data, ObjInt address)
{
    // weeding
    auto test1 = data->sections.begin();
    ObjSection* test2 = (*test1).section;
    if (!manager->HasVirtual(test2->GetName()))
    {
        data->sections.clear();
        return 0;
    }
    ObjSection* curSection = nullptr;
    ObjFile* curFile = nullptr;
    for (auto item : data->sections)
    {
        ObjSection* sect = item.section;
        if (!curSection || sect->GetAbsSize() > curSection->GetAbsSize())
        {
            curSection = sect;
            curFile = item.file;
        }
    }
    if (!curSection)
        return 0;
    for (auto item : data->sections)
    {
        ObjSection* sect = item.section;
        sect->SetAliasFor(curSection);
    }
    data->sections.clear();
    data->sections.push_back(OneSection(curFile, curSection));

    curSection->SetBase(address);
    if (attribs.GetVirtualOffsetSpecified())
        curSection->SetVirtualOffset(attribs.GetVirtualOffset());
    // make a public for virtual sections (C++)
    if (curSection->GetQuals() & ObjSection::virt)
    {
        int i;
        for (i = 0; i < curSection->GetName().size(); i++)
        {
            if (curSection->GetName()[i] == '@' || curSection->GetName()[i] == '_')
                break;
        }
        if (i < curSection->GetName().size())
        {
            std::string pubName = curSection->GetName().substr(i);
            LinkExpressionSymbol* esym = new LinkExpressionSymbol(pubName, new LinkExpression(curSection));
            if (!LinkExpression::EnterSymbol(esym))
            {
                delete esym;
                LinkManager::LinkError("Symbol " + pubName + " redefined");
            }
            else
            {
                manager->EnterVirtualSection(curSection);
            }
        }
    }
    return curSection->GetAbsSize();
}
ObjInt LinkRegion::ArrangeSections(LinkManager* manager)
{
    ObjInt address = attribs.GetAddress(), oldAddress = address;
    ObjInt size;
    int align = attribs.GetAlign();
    address += align - 1;
    address /= align;
    address *= align;
    attribs.SetAddress(new LinkExpression(address));
    if (common)
    {
        for (auto& data : nowData)
        {
            for (auto item : data->sections)
            {
                ObjSection* sect = item.section;
                sect->SetBase(address);
                if (attribs.GetVirtualOffsetSpecified())
                    sect->SetVirtualOffset(attribs.GetVirtualOffset());
            }
        }
        for (auto& data : normalData)
        {
            for (auto item : data->sections)
            {
                ObjSection* sect = item.section;
                sect->SetBase(address);
                if (attribs.GetVirtualOffsetSpecified())
                    sect->SetVirtualOffset(attribs.GetVirtualOffset());
            }
        }
        for (auto& data : postponeData)
        {
            for (auto item : data->sections)
            {
                ObjSection* sect = item.section;
                sect->SetBase(address);
                if (attribs.GetVirtualOffsetSpecified())
                    sect->SetVirtualOffset(attribs.GetVirtualOffset());
            }
        }
        size = maxSize;
    }
    else if (overlayed)
    {
        // it is an error if an overlayed section appears in multiple categories
        // and for non-virtual sections it won't be flagged.   For virtual sections
        // the implicit public will be redeclared and cause an error...
        for (auto& data : nowData)
        {
            address += align - 1;
            address /= align;
            address *= align;
            address += ArrangeOverlayed(manager, data.get(), address);
        }
        for (auto& data : normalData)
        {
            address += align - 1;
            address /= align;
            address *= align;
            address += ArrangeOverlayed(manager, data.get(), address);
        }
        for (auto& data : postponeData)
        {
            address += align - 1;
            address /= align;
            address *= align;
            address += ArrangeOverlayed(manager, data.get(), address);
        }
        size = address - oldAddress;
    }
    else
    {
        for (auto& data : nowData)
        {
            for (auto item : data->sections)
            {
                ObjSection* sect = item.section;
                address += align - 1;
                address /= align;
                address *= align;
                sect->SetBase(address);
                if (attribs.GetVirtualOffsetSpecified())
                    sect->SetVirtualOffset(attribs.GetVirtualOffset() + address - oldAddress);
                address += sect->GetAbsSize();
            }
        }
        for (auto& data : normalData)
        {
            for (auto item : data->sections)
            {
                ObjSection* sect = item.section;
                address += align - 1;
                address /= align;
                address *= align;
                sect->SetBase(address);
                if (attribs.GetVirtualOffsetSpecified())
                    sect->SetVirtualOffset(attribs.GetVirtualOffset() + address - oldAddress);
                address += sect->GetAbsSize();
            }
        }
        for (auto& data : postponeData)
        {
            for (auto item : data->sections)
            {
                ObjSection* sect = item.section;
                address += align - 1;
                address /= align;
                address *= align;
                sect->SetBase(address);
                if (attribs.GetVirtualOffsetSpecified())
                    sect->SetVirtualOffset(attribs.GetVirtualOffset() + address - oldAddress);
                address += sect->GetAbsSize();
            }
        }
        size = address - oldAddress;
    }
    if (attribs.GetRoundSize())
    {
        int rs = attribs.GetRoundSize();
        if (rs < 1)
            rs = 1;
        size = size + rs - 1;
        size = size / rs;
        size = size * rs;
    }
    if (attribs.GetSize() && size > attribs.GetSize())
        LinkManager::LinkError("Region " + QualifiedRegionName() + " overflowed region size");
    return size;
}
ObjInt LinkRegion::PlaceRegion(LinkManager* manager, LinkAttribs& partitionAttribs, ObjInt base)
{
    int alignAdjust = 0;
    if (!attribs.GetVirtualOffsetSpecified())
        if (partitionAttribs.GetVirtualOffsetSpecified())
            attribs.SetVirtualOffset(
                new LinkExpression(partitionAttribs.GetVirtualOffset() + base - partitionAttribs.GetAddress()));
    if (partitionAttribs.GetAlign() > attribs.GetAlign())
        attribs.SetAlign(new LinkExpression(partitionAttribs.GetAlign()));
    //	else if (attribs.GetAlign() > partitionAttribs.GetAlign())
    //		partitionAttribs.SetAlign(new LinkExpression(attribs.GetAlign()));
    if (!attribs.GetHasFill())
        if (partitionAttribs.GetHasFill())
            attribs.SetFill(new LinkExpression(partitionAttribs.GetFill()));

    if (attribs.GetAlign())
    {
        int al = attribs.GetAlign();
        if (al < 1)
            al = 1;
        alignAdjust = base % al;
        if (alignAdjust)
            alignAdjust = al - alignAdjust;
    }
    if (!attribs.GetAddressSpecified())
    {
        attribs.SetAddress(new LinkExpression(base + alignAdjust - partitionAttribs.GetAddress()));
    }
    else
    {
        if (attribs.GetAddress() < base)
            LinkManager::LinkError("Region " + QualifiedRegionName() + " overlaps other regions ");
    }
    CheckAttributes();
    int size = ArrangeSections(manager);
    if (size < attribs.GetSize())
        size = attribs.GetSize();

    if (attribs.GetRoundSize())
    {
        int rs = attribs.GetRoundSize();
        if (rs < 1)
            rs = 1;
        size = size + rs - 1;
        size = size / rs;
        size = size * rs;
    }
    if (attribs.GetMaxSize())
        if (size > attribs.GetMaxSize())
            LinkManager::LinkError("Region " + QualifiedRegionName() + " exceeds maximum size ");

    attribs.SetSize(new LinkExpression(size));
    return size + alignAdjust;
}
