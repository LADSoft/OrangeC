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
#include <ctype.h>

LinkRegion::~LinkRegion()
{
    for (std::vector<NamedSection *>:: iterator it =
         nowData.begin(); it != nowData.end(); it++)
    {
        NamedSection *n = (*it);
        delete n;
    }
    for (std::vector<NamedSection *>:: iterator it =
         normalData.begin(); it != normalData.end(); it++)
    {
        NamedSection *n = (*it);
        delete n;
    }
    for (std::vector<NamedSection *>:: iterator it =
         postponeData.begin(); it != postponeData.end(); it++)
    {
        NamedSection *n = (*it);
        delete n;
    }
}
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
bool LinkRegion::ParseFiles(CmdFiles &files, LinkTokenizer &spec)
{
    
    if (!spec.Matches(LinkTokenizer::eBegin))
        return false;
    bool done = false;
    const char *p = spec.GetData();
    const char *s = p;
    while (!done)
    {
        while (isspace(*s)) s++;
        char buf[256], *q = buf;
        bool quote = false;
        if (*s == '"')
        {
            s++;
            while (*s && *s != '}' && *s != '"') *q++ = *s++;
        }
        else
        {
            while (*s && *s != '}' && !isspace(*s)) *q ++ = *s++;
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
bool LinkRegion::ParseName(LinkTokenizer &spec)
{
    char buf[4096], *q = buf;
    const char *p = spec.GetData();
    while (isspace(*p) || UTF8::IsAlnum(p) || *p == '*' || *p == '?' ||
           *p == '|' || *p == '&' || *p == '!' || *p == '(' || *p == ')')
    {
        int v = UTF8::CharSpan(p);
        for (int i=0; i < v && *p; i++)
            *q++ = *p++;
    }
    *q = 0;
    spec.Skip(q - buf);
    SetName(buf);
    return true;
}
bool LinkRegion::ParseValue(LinkTokenizer &spec, LinkExpression **rv)
{
    spec.NextToken();
    if (!spec.MustMatch(LinkTokenizer::eAssign))
        return false;
    if (!spec.GetExpression(rv,false))
        return false;
    return true;
}
bool LinkRegion::ParseAttributes( LinkTokenizer &spec)
{
    /* optional */
    if (spec.Matches(LinkTokenizer::eBracketOpen))
    {
        bool done = false;
        spec.NextToken();
        while (!done)
        {
            switch(spec.GetTokenType())
            {
                LinkExpression *value;
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
        return spec.MustMatch(LinkTokenizer:: eBracketClose);
    }
    return true;
}
bool LinkRegion::ParseRegionSpec(LinkManager *manager, CmdFiles &files, LinkTokenizer &spec)
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
bool LinkRegion::HoldsFile(const ObjString &fileName)
{
    for (SourceFileIterator it = sourceFiles.begin(); it != sourceFiles.end(); ++it)
    {
        if (*(*it) == fileName)
            return true;
    }
    return false;
}
bool LinkRegion::Matches(const ObjString &name, const ObjString &spec)
{
    LinkRegionFileSpecContainer fs(spec);
    return fs.Matches(name);
}
void LinkRegion::AddSourceFile(CmdFiles &filelist, const ObjString &spec)
{
    for (CmdFiles::FileNameIterator it = filelist.FileNameBegin(); it != filelist.FileNameEnd(); ++it)		
        if (Matches(**it, spec))
            sourceFiles.push_back(*it);
}
void LinkRegion::AddData(SectionData &data, ObjFile *file, ObjSection *section)
{
    NamedSection *ns = NULL;
    for (std::vector<NamedSection *>::iterator it = data.begin(); it != data.end(); ++it)
    {
        if ((*it)->name == section->GetName())
        {
            ns = *it;
            break;
        }
    }
    if (ns == NULL)
    {
        ns = new NamedSection;
        ns->name = section->GetName();
        data.push_back(ns);
    }
    ns->sections.push_back(OneSection(file, section));
}
void LinkRegion::AddFile(ObjFile *file)
{
    LinkNameLogic logic(name);
    for (ObjFile::SectionIterator it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        ObjSection *sect = *it;
        if (logic.Matches(sect->GetName()))
        {
            ObjInt quals = sect->GetQuals();
            if (attribs.GetAlign() < sect->GetAlignment())
                attribs.SetAlign(new LinkExpression(sect->GetAlignment()));
            // using section::utility flag to for placed regions
            sect->SetUtilityFlag(true);
            if (quals & ObjSection::now)
            {
                if (quals &ObjSection::postpone)
                    LinkManager::LinkError("file " + file->GetName() + "Region " 
                                             + QualifiedRegionName() + 
                                "has both 'now' and 'postpone' qualifiers");
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
void LinkRegion::AddSection(LinkManager *manager)
{
    for (LinkManager::FileIterator it = manager->FileBegin(); it != manager->FileEnd(); ++it)
    {
        ObjFile *file = *it;
        if (sourceFiles.size() == 0)
            AddFile(file);
        else for (CmdFiles::FileNameIterator it = sourceFiles.begin(); it != sourceFiles.end(); ++it)
        {
            if (**it == file->GetInputName())
            {
                AddFile(file);
                break;
            }
        }
    }
}
#define max(x,y) (((x)>(y))?(x):(y))
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
        ObjInt maxAlign = -1;
        for (SectionDataIterator it1 = nowData.begin(); it1 != nowData.end(); ++it1)
        {
            for (OneSectionIterator it = (*it1)->sections.begin(); it != (*it1)->sections.end(); ++it)
            {
                ObjSection *sect = (*it).section;
                ObjInt quals = sect->GetQuals();
                if (maxSize != -1 && maxSize != sect->GetAbsSize())
                    notEqual = true;
                maxSize = max(maxSize, sect->GetAbsSize());
                maxAlign = max(maxAlign, sect->GetAlignment());
                if (quals &ObjSection::equal)
                {
                    anyEqual = true;
                    anyCommon = true;
                }
                else if (quals &ObjSection::max)
                {
                    anyMax = true;
                    anyCommon = true;
                }
                else if (quals & ObjSection::common)
                    anyCommon = true;
                else 
                    anyNonCommon = true;
                if (quals &ObjSection::unique)
                    anyUnique = true;
                if (quals &ObjSection::separate)
                    anySeparate = true;
            }
        }
        for (SectionDataIterator it1 = normalData.begin(); it1 != normalData.end(); ++it1)
        {
            for (OneSectionIterator it = (*it1)->sections.begin(); it != (*it1)->sections.end(); ++it)
            {
                ObjSection *sect = (*it).section;
                ObjInt quals = sect->GetQuals();
                if (maxSize != -1 && maxSize != sect->GetAbsSize())
                    notEqual = true;
                maxSize = max(maxSize, sect->GetAbsSize());
                maxAlign = max(maxAlign, sect->GetAlignment());
                if (quals &ObjSection::equal)
                {
                    anyEqual = true;
                    anyCommon = true;
                }
                else if (quals &ObjSection::max)
                {
                    anyMax = true;
                    anyCommon = true;
                }
                else if (quals & ObjSection::common)
                    anyCommon = true;
                else 
                    anyNonCommon = true;
                if (quals &ObjSection::unique)
                    anyUnique = true;
                if (quals &ObjSection::separate)
                    anySeparate = true;
            }
        }
        for (SectionDataIterator it1 = postponeData.begin(); it1 != postponeData.end(); ++it1)
        {
            for (OneSectionIterator it = (*it1)->sections.begin(); it != (*it1)->sections.end(); ++it)
            {
                ObjSection *sect = (*it).section;
                ObjInt quals = sect->GetQuals();
                if (maxSize != -1 && maxSize != sect->GetAbsSize())
                    notEqual = true;
                maxSize = max(maxSize, sect->GetAbsSize());
                maxAlign = max(maxAlign, sect->GetAlignment());
                if (quals &ObjSection::equal)
                {
                    anyEqual = true;
                }
                else if (quals &ObjSection::max)
                {
                    anyMax = true;
                }
                else if (quals & ObjSection::common)
                    anyCommon = true;
                else 
                    anyNonCommon = true;
                if (quals &ObjSection::unique)
                    anyUnique = true;
                if (quals &ObjSection::separate)
                    anySeparate = true;
            }
        }
        if (anyCommon)
            common = true;
        if (n > 1 && anyUnique)
            LinkManager::LinkError("Region " + QualifiedRegionName() + " unique qualifier with multiple sections");
        if ((anyCommon && anyNonCommon)
            || (anyEqual && anyNonCommon)
            || (anyMax && anyNonCommon))
            LinkManager::LinkError("Region " + QualifiedRegionName() + " Mixing Common and NonCommon sections");
        if (anyEqual && anyMax)
            LinkManager::LinkError("Region " + QualifiedRegionName() + " Mixing equal and max characteristics");
        if (anySeparate && anyCommon)
            LinkManager::LinkError("Region " + QualifiedRegionName() + " Mixing separate and common sections");
        if (anyEqual && notEqual)
            LinkManager::LinkError("Region " + QualifiedRegionName() + " equal qualifier with unequal sections");
        if (maxAlign > attribs.GetAlign())
            attribs.SetAlign(new LinkExpression(maxAlign));
    }
}
ObjInt LinkRegion::ArrangeSections()
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
        for (SectionDataIterator it = nowData.begin(); it != nowData.end(); ++it)
        {
            for (OneSectionIterator it1 = (*it)->sections.begin(); it1 != (*it)->sections.end(); ++it1)
            {
                ObjSection *sect = (*it1).section;
                sect->SetBase(address);
                if (attribs.GetVirtualOffsetSpecified())
                    sect->SetVirtualOffset(attribs.GetVirtualOffset());
            }
        }
        for (SectionDataIterator it = normalData.begin(); it != normalData.end(); ++it)
        {
            for (OneSectionIterator it1 = (*it)->sections.begin(); it1 != (*it)->sections.end(); ++it1)
            {
                ObjSection *sect = (*it1).section;
                sect->SetBase(address);
                if (attribs.GetVirtualOffsetSpecified())
                    sect->SetVirtualOffset(attribs.GetVirtualOffset());
            }
        }
        for (SectionDataIterator it = postponeData.begin(); it != postponeData.end(); ++it)
        {
            for (OneSectionIterator it1 = (*it)->sections.begin(); it1 != (*it)->sections.end(); ++it1)
            {
                ObjSection *sect = (*it1).section;
                sect->SetBase(address);
                if (attribs.GetVirtualOffsetSpecified())
                    sect->SetVirtualOffset(attribs.GetVirtualOffset());
            }
        }
        size = maxSize;
    }
    else
    {
        for (SectionDataIterator it = nowData.begin(); it != nowData.end(); ++it)
        {
            for (OneSectionIterator it1 = (*it)->sections.begin(); it1 != (*it)->sections.end(); ++it1)
            {
                ObjSection *sect = (*it1).section;
                address += align - 1;
                address /= align;
                address *= align;
                sect->SetBase(address);
                if (attribs.GetVirtualOffsetSpecified())
                    sect->SetVirtualOffset(attribs.GetVirtualOffset() + address - oldAddress);
                address += sect->GetAbsSize();
            }
        }
        for (SectionDataIterator it = normalData.begin(); it != normalData.end(); ++it)
        {
            for (OneSectionIterator it1 = (*it)->sections.begin(); it1 != (*it)->sections.end(); ++it1)
            {
                ObjSection *sect = (*it1).section;
                address += align - 1;
                address /= align;
                address *= align;
                sect->SetBase(address);
                if (attribs.GetVirtualOffsetSpecified())
                    sect->SetVirtualOffset(attribs.GetVirtualOffset() + address - oldAddress);
                address += sect->GetAbsSize();
            }
        }
        for (SectionDataIterator it = postponeData.begin(); it != postponeData.end(); ++it)
        {
            for (OneSectionIterator it1 = (*it)->sections.begin(); it1 != (*it)->sections.end(); ++it1)
            {
                ObjSection *sect = (*it1).section;
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
        size = size + attribs.GetRoundSize() - 1;
        size = size / attribs.GetRoundSize();
        size = size * attribs.GetRoundSize();
    }
    if (attribs.GetSize() && size > attribs.GetSize())
        LinkManager::LinkError("Region " + QualifiedRegionName() + " overflowed region size");
    return size;
}
ObjInt LinkRegion::PlaceRegion(LinkAttribs &partitionAttribs, ObjInt base)
{
    int alignAdjust;
    if (!attribs.GetVirtualOffsetSpecified())
        if (partitionAttribs.GetVirtualOffsetSpecified())
            attribs.SetVirtualOffset(new LinkExpression(partitionAttribs.GetVirtualOffset() + base - partitionAttribs.GetAddress()));
    if (partitionAttribs.GetAlign() > attribs.GetAlign())
        attribs.SetAlign(new LinkExpression(partitionAttribs.GetAlign()));
//	else if (attribs.GetAlign() > partitionAttribs.GetAlign())
//		partitionAttribs.SetAlign(new LinkExpression(attribs.GetAlign()));
    if (!attribs.GetHasFill())
        if (partitionAttribs.GetHasFill())
            attribs.SetFill(new LinkExpression(partitionAttribs.GetFill()));
            
    if (attribs.GetAlign())
    {
        alignAdjust = base % attribs.GetAlign();
        if (alignAdjust)
            alignAdjust = attribs.GetAlign() - alignAdjust;
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
    int size = ArrangeSections();
    if (size < attribs.GetSize())
        size = attribs.GetSize();

    if (attribs.GetRoundSize())
    {
        size = size + attribs.GetRoundSize() - 1;
        size = size / attribs.GetRoundSize();
        size = size * attribs.GetRoundSize();
    }
    if (attribs.GetMaxSize())
        if (size > attribs.GetMaxSize())
            LinkManager::LinkError("Region " + QualifiedRegionName() + " exceeds maximum size ");
            
    attribs.SetSize(new LinkExpression(size));
    return size + alignAdjust;
}
