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

#include "LinkPartition.h"
#include "LinkOverlay.h"
#include "LinkRegion.h"
#include "LinkTokenizer.h"
#include "LinkExpression.h"
#include "LinkManager.h"
#include "CmdFiles.h"
#include "ObjFile.h"
#include "ObjSection.h"

LinkOverlaySpecifier::~LinkOverlaySpecifier() {}
LinkPartition::~LinkPartition() {}
void LinkPartition::Add(LinkOverlaySpecifier* ov)
{
    std::unique_ptr<LinkOverlaySpecifier> temp(ov);
    overlays.push_back(std::move(temp));
}

bool LinkPartition::ParseName(LinkTokenizer& spec)
{
    if (!spec.Matches(LinkTokenizer::eSymbol))
        return false;
    SetName(spec.GetSymbol());
    spec.NextToken();
    return true;
}
bool LinkPartition::ParseValue(LinkTokenizer& spec, LinkExpression** rv, bool alreadyassign)
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
bool LinkPartition::ParseAttributes(LinkTokenizer& spec)
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
        return spec.MustMatch(LinkTokenizer::eBracketClose);
    }
    return true;
}
bool LinkPartition::ParseAssignment(LinkTokenizer& spec)
{
    ObjString symName = spec.GetSymbol();
    LinkExpression* value;
    spec.NextToken();
    if (!spec.MustMatch(LinkTokenizer::eAssign))
        return false;
    if (!spec.GetExpression(&value, true))
        return false;
    LinkExpressionSymbol* esym = new LinkExpressionSymbol(symName, value);
    if (!LinkExpression::EnterSymbol(esym))
    {
        delete esym;
        LinkManager::LinkError("Symbol " + symName + " redefined");
    }
    else
    {
        overlays.push_back(std::make_unique<LinkOverlaySpecifier>(esym));
    }
    return spec.MustMatch(LinkTokenizer::eSemi);
}
bool LinkPartition::CreateSeparateRegions(LinkManager* manager, CmdFiles& files, LinkTokenizer& spec)
{
    std::unique_ptr<LinkOverlay> newOverlay = std::make_unique<LinkOverlay>(this);
    std::unique_ptr<LinkRegion> newRegion = std::make_unique<LinkRegion>(newOverlay.get());
    if (!newRegion->ParseRegionSpec(manager, files, spec))
        return false;
    if (!spec.MustMatch(LinkTokenizer::eSemi))
        return false;
    for (auto itr = newRegion->NowDataBegin(); itr != newRegion->NowDataEnd(); ++itr)
    {
        for (auto sect : (*itr)->sections)
        {
            LinkOverlay* overlay = new LinkOverlay(this);
            overlay->SetName(std::string(sect.file->GetName()) + "_" + sect.section->GetName());
            overlays.push_back(std::make_unique<LinkOverlaySpecifier>(overlay));
            LinkRegion* region = new LinkRegion(overlay);
            overlay->Add(new LinkRegionSpecifier(region));
            region->SetName(newRegion->GetName());
            region->SetAttribs(newRegion->GetAttribs());
            region->AddNormalData(sect.file, sect.section);
        }
    }
    for (auto itr = newRegion->NormalDataBegin(); itr != newRegion->NormalDataEnd(); ++itr)
    {
        for (auto sect : (*itr)->sections)
        {
            LinkOverlay* overlay = new LinkOverlay(this);
            overlay->SetName(std::string(sect.file->GetName()) + "_" + sect.section->GetName());
            overlays.push_back(std::make_unique<LinkOverlaySpecifier>(overlay));
            LinkRegion* region = new LinkRegion(overlay);
            overlay->Add(new LinkRegionSpecifier(region));
            region->SetName(newRegion->GetName());
            region->SetAttribs(newRegion->GetAttribs());
            region->AddNormalData(sect.file, sect.section);
        }
    }
    for (auto itr = newRegion->PostponeDataBegin(); itr != newRegion->PostponeDataEnd(); ++itr)
    {
        for (auto sect : (*itr)->sections)
        {
            LinkOverlay* overlay = new LinkOverlay(this);
            overlay->SetName(std::string(sect.file->GetName()) + "_" + sect.section->GetName());
            overlays.push_back(std::make_unique<LinkOverlaySpecifier>(overlay));
            LinkRegion* region = new LinkRegion(overlay);
            overlay->Add(new LinkRegionSpecifier(region));
            region->SetName(newRegion->GetName());
            region->SetAttribs(newRegion->GetAttribs());
            region->AddNormalData(sect.file, sect.section);
        }
    }
    return true;
}
bool LinkPartition::ParseOverlays(LinkManager* manager, CmdFiles& files, LinkTokenizer& spec)
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
            LinkOverlay* newOverlay = new LinkOverlay(this);
            overlays.push_back(std::make_unique<LinkOverlaySpecifier>(newOverlay));
            if (!newOverlay->ParseOverlaySpec(manager, files, spec))
                return false;
            if (!spec.MustMatch(LinkTokenizer::eSemi))
                return false;
        }
    }
    return spec.MustMatch(LinkTokenizer::eEnd);
}
bool LinkPartition::ParsePartitionSpec(LinkManager* manager, CmdFiles& files, LinkTokenizer& spec)
{
    if (!ParseOverlays(manager, files, spec))
        return false;
    if (!ParseName(spec))
        return false;
    return ParseAttributes(spec);
}
#define max(x, y) (((x) > (y)) ? (x) : (y))
ObjInt LinkPartition::PlacePartition(LinkManager* manager, ObjInt bottom, bool completeLink, int& overlayNum)
{
    ObjInt maxSize = 0;
    if (!attribs.GetAddressSpecified())
    {
        attribs.SetAddress(new LinkExpression(bottom));
    }
    bottom = attribs.GetAddress();
    ObjInt size = 0;
    for (auto& overlay : overlays)
    {
        if (overlay->GetSymbol())
        {
            overlay->GetSymbol()->SetValue(overlay->GetSymbol()->GetValue()->Eval(overlayNum ? overlayNum - 1 : 0, bottom, size));
        }
        else
        {
            size = overlay->GetOverlay()->PlaceOverlay(manager, attribs, completeLink, overlayNum++);
            maxSize = max(size, maxSize);
        }
    }
    if (attribs.GetRoundSize())
    {
        maxSize += attribs.GetRoundSize() - 1;
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
