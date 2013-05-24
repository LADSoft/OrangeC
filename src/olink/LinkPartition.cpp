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
#include "LinkPartition.h"
#include "LinkOverlay.h"
#include "LinkRegion.h"
#include "LinkTokenizer.h"
#include "LinkExpression.h"
#include "LinkManager.h"
#include "CmdFiles.h"
#include "ObjFile.h"
#include "ObjSection.h"

LinkPartition::~LinkPartition()
{
    for (OverlayIterator it = overlays.begin(); it != overlays.end(); it++)
    {
        LinkOverlaySpecifier *s = *it;
        delete s;
    }
}
bool LinkPartition::ParseName(LinkTokenizer &spec)
{
    if (!spec.Matches(LinkTokenizer::eSymbol))
        return false;
    SetName(spec.GetSymbol());
    spec.NextToken();
    return true;
}
bool LinkPartition::ParseValue(LinkTokenizer &spec, LinkExpression **rv, bool alreadyassign)
{
    if (!alreadyassign)
    {
        spec.NextToken();
    }
    if (!spec.MustMatch(LinkTokenizer::eAssign))
        return false;
    if (!spec.GetExpression(rv, true))
        return false;
    return true;
}
bool LinkPartition::ParseAttributes( LinkTokenizer &spec)
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
                case LinkTokenizer::eVirtual:
                    if (!ParseValue(spec, &value))
                        return false;
                    attribs.SetVirtualOffset(value);
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
bool LinkPartition::ParseAssignment(LinkTokenizer &spec)
{
    ObjString symName = spec.GetSymbol();
    LinkExpression *value;
    spec.NextToken();
    if (!spec.MustMatch(LinkTokenizer::eAssign))
        return false;
    if (!spec.GetExpression(&value, true))
        return false;
    LinkExpressionSymbol *esym = new LinkExpressionSymbol(symName, value);
    if (!LinkExpression::EnterSymbol(esym))
    {
        delete esym;
        LinkManager::LinkError("Symbol " + symName + " redefined");
    }
    else
    {
        overlays.push_back(new LinkOverlaySpecifier(esym));
    }
    return spec.MustMatch(LinkTokenizer::eSemi);
}
bool LinkPartition::CreateSeparateRegions(LinkManager *manager, CmdFiles &files, LinkTokenizer &spec)
{
    LinkOverlay *newOverlay = new LinkOverlay(this);
    LinkRegion *newRegion = new LinkRegion(newOverlay);
    if (!newRegion->ParseRegionSpec(manager, files, spec))
        return false;
    if (!spec.MustMatch(LinkTokenizer::eSemi))
        return false;	
    for (LinkRegion::SectionDataIterator itr = newRegion->NowDataBegin();
         itr != newRegion->NowDataEnd(); ++itr)
    {
        for (LinkRegion::OneSectionIterator it = (*itr)->sections.begin();
             it != (*itr)->sections.end(); ++ it)
        {
            LinkOverlay *overlay = new LinkOverlay(this);
            overlay->SetName(std::string((*it).file->GetName()) + "_" + (*it).section->GetName());
            overlays.push_back(new LinkOverlaySpecifier(overlay));
            LinkRegion *region = new LinkRegion(overlay);
            overlay->Add(new LinkRegionSpecifier(region));
            region->SetName(newRegion->GetName());
            region->SetAttribs(newRegion->GetAttribs());
            region->AddNormalData((*it).file, (*it).section);
        }
    }
    for (LinkRegion::SectionDataIterator itr = newRegion->NormalDataBegin();
         itr != newRegion->NormalDataEnd(); ++itr)
    {
        for (LinkRegion::OneSectionIterator it = (*itr)->sections.begin();
             it != (*itr)->sections.end(); ++ it)
        {
            LinkOverlay *overlay = new LinkOverlay(this);
            overlay->SetName(std::string((*it).file->GetName()) + "_" + (*it).section->GetName());
            overlays.push_back(new LinkOverlaySpecifier(overlay));
            LinkRegion *region = new LinkRegion(overlay);
            overlay->Add(new LinkRegionSpecifier(region));
            region->SetName(newRegion->GetName());
            region->SetAttribs(newRegion->GetAttribs());
            region->AddNormalData((*it).file, (*it).section);
        }
    }
    for (LinkRegion::SectionDataIterator itr = newRegion->PostponeDataBegin();
         itr != newRegion->PostponeDataEnd(); ++itr)
    {
        for (LinkRegion::OneSectionIterator it = (*itr)->sections.begin();
             it != (*itr)->sections.end(); ++ it)
        {
            LinkOverlay *overlay = new LinkOverlay(this);
            overlay->SetName(std::string((*it).file->GetName()) + "_" + (*it).section->GetName());
            overlays.push_back(new LinkOverlaySpecifier(overlay));
            LinkRegion *region = new LinkRegion(overlay);
            overlay->Add(new LinkRegionSpecifier(region));
            region->SetName(newRegion->GetName());
            region->SetAttribs(newRegion->GetAttribs());
            region->AddNormalData((*it).file, (*it).section);
        }
    }
    delete newRegion;
    delete newOverlay;
    return true;
}
bool LinkPartition::ParseOverlays(LinkManager *manager, CmdFiles &files, LinkTokenizer &spec)
{
    if (!spec.MustMatch(LinkTokenizer::eBegin))
        return false;
    bool done = false;
    while (!done)
    {
        if (spec.Matches(LinkTokenizer::eSymbol))
        {
            if (!ParseAssignment(spec))
                return false;
            if (!spec.MustMatch(LinkTokenizer::eSemi))
                return false;	
        }
        else if (spec.Matches(LinkTokenizer::eRegion))
        {
            spec.NextToken();
            if (!CreateSeparateRegions(manager, files, spec))
                return false;
        }
        else if (!spec.MustMatch(LinkTokenizer::eOverlay))
        {
            done = true;
        }
        else
        {
            LinkOverlay *newOverlay = new LinkOverlay(this);
            overlays.push_back(new LinkOverlaySpecifier(newOverlay));
            if (!newOverlay->ParseOverlaySpec(manager, files, spec))
                return false;
            if (!spec.MustMatch(LinkTokenizer::eSemi))
                return false;	
        }
    }
    return spec.MustMatch(LinkTokenizer::eEnd);
}
bool LinkPartition::ParsePartitionSpec(LinkManager *manager, CmdFiles &files, LinkTokenizer &spec)
{
    if (!ParseOverlays(manager, files, spec))
        return false;
    if (!ParseName(spec))
        return false;
    return ParseAttributes(spec);
}
#define max(x,y) (((x)>(y)) ? (x) : (y))
ObjInt LinkPartition::PlacePartition(ObjInt bottom, bool completeLink, int &overlayNum)
{
    ObjInt maxSize = 0;
    if (!attribs.GetAddressSpecified())
    {
        attribs.SetAddress(new LinkExpression(bottom));
    }
    bottom = attribs.GetAddress();
    ObjInt size = 0;
    for (OverlayIterator it = overlays.begin(); it != overlays.end(); ++it)
    {
        if ((*it)->GetSymbol())
        {
//			if (completeLink)
//				(*it)->GetSymbol()->SetValue(new LinkExpression((*it)->GetSymbol()->GetValue()->Eval(bottom + size)));
//			else
                (*it)->GetSymbol()->SetValue((*it)->GetSymbol()->GetValue()->Eval(overlayNum ? overlayNum -1 : 0, bottom, size));
        }
        else
        {
            size = (*it)->GetOverlay()->PlaceOverlay(attribs, completeLink, overlayNum++);
            maxSize = max(size, maxSize);
        }
    }
    if (attribs.GetRoundSize())
    {
        maxSize += attribs.GetRoundSize() -1;
        maxSize /= attribs.GetRoundSize();
        maxSize *= attribs.GetRoundSize();
    }
    if (attribs.GetSize() && maxSize > attribs.GetSize())
        LinkManager::LinkError("Partition " + name + " overflowed partition size");
    if (attribs.GetMaxSize())
        if (maxSize > attribs.GetMaxSize())
            LinkManager::LinkError("Partition " + name + " exceeds maximum size ");
    attribs.SetSize(new LinkExpression(maxSize));
    return bottom + maxSize;
}
