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

#include "LinkManager.h"
#include "LinkOverlay.h"
#include "LinkRegion.h"
#include "LinkTokenizer.h"
#include "LinkExpression.h"
#include "CmdFiles.h"

LinkRegionSpecifier::~LinkRegionSpecifier() {}
LinkOverlay::~LinkOverlay() {}
void LinkOverlay::Add(LinkRegionSpecifier* region)
{
    std::unique_ptr<LinkRegionSpecifier> temp(region);
    regions.push_back(std::move(temp));
}
bool LinkOverlay::ParseAssignment(LinkTokenizer& spec)
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
        regions.push_back(std::make_unique<LinkRegionSpecifier>(esym));
    }
    return spec.MustMatch(LinkTokenizer::eSemi);
}
bool LinkOverlay::ParseName(LinkTokenizer& spec)
{
    if (!spec.Matches(LinkTokenizer::eSymbol))
        return false;
    SetName(spec.GetSymbol());
    spec.NextToken();
    return true;
}
bool LinkOverlay::ParseValue(LinkTokenizer& spec, LinkExpression** rv, bool alreadyassign)
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
bool LinkOverlay::ParseAttributes(LinkTokenizer& spec)
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
bool LinkOverlay::ParseOverlaySpec(LinkManager* manager, CmdFiles& files, LinkTokenizer& spec)
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
        }
        else if (!spec.MustMatch(LinkTokenizer::eRegion))
        {
            done = true;
        }
        else
        {
            LinkRegion* newRegion = new LinkRegion(this);
            regions.push_back(std::make_unique<LinkRegionSpecifier>(newRegion));
            if (!newRegion->ParseRegionSpec(manager, files, spec))
                return false;
            if (!spec.MustMatch(LinkTokenizer::eSemi))
                return false;
        }
    }
    if (!spec.MustMatch(LinkTokenizer::eEnd))
        return false;
    if (!ParseName(spec))
        return false;
    return ParseAttributes(spec);
}
ObjInt LinkOverlay::PlaceOverlay(LinkManager* manager, LinkAttribs& partitionAttribs, bool completeLink, int overlayNum)
{
    ObjInt size = 0;
    ObjInt base = partitionAttribs.GetAddress();
    ObjInt alignRegion0 = 1;
    for (auto& region : regions)
    {
        if (!region->GetSymbol())
        {
            alignRegion0 = region->GetRegion()->GetAttribs().GetAlign();
            break;
        }
    }
    if (alignRegion0 != 1)
        base = (base + (alignRegion0 - 1)) & ~(alignRegion0 - 1);
    if (attribs.GetAddressSpecified())
        base = attribs.GetAddress();
    else
        attribs.SetAddress(new LinkExpression(base));
    if (partitionAttribs.GetVirtualOffsetSpecified() && !attribs.GetVirtualOffsetSpecified())
        attribs.SetVirtualOffset(new LinkExpression(partitionAttribs.GetVirtualOffset()));
    if (partitionAttribs.GetAlign() > attribs.GetAlign())
        attribs.SetAlign(new LinkExpression(partitionAttribs.GetAlign()));
    if (attribs.GetSize() == 0)
        attribs.SetSize(new LinkExpression(partitionAttribs.GetSize()));
    if (!attribs.GetHasFill())
        if (partitionAttribs.GetHasFill())
            attribs.SetFill(new LinkExpression(partitionAttribs.GetFill()));
    for (auto& region : regions)
    {
        if (region->GetSymbol())
        {
            //			if (completeLink)
            //				region->GetSymbol()->SetValue(new LinkExpression(region->GetSymbol()->GetValue()->Eval(base + size)));
            //			else
            region->GetSymbol()->SetValue(region->GetSymbol()->GetValue()->Eval(overlayNum, base, size));
        }
        else
        {
            size += region->GetRegion()->PlaceRegion(manager, attribs, base + size);
        }
    }
    //	if (attribs.GetAlign() > partitionAttribs.GetAlign())
    //		partitionAttribs.SetAlign(new LinkExpression(attribs.GetAlign()));
    if (size < attribs.GetSize())
        size = attribs.GetSize();
    if (!attribs.GetRoundSize())
        if (partitionAttribs.GetRoundSize())
            attribs.SetRoundSize(new LinkExpression(partitionAttribs.GetRoundSize()));
    if (attribs.GetRoundSize())
    {
        size += attribs.GetRoundSize() - 1;
        size /= attribs.GetRoundSize();
        size *= attribs.GetRoundSize();
    }
    if (attribs.GetSize() && size > attribs.GetSize())
        LinkManager::LinkError("Overlay " + name + " overflowed overlay size");
    if (attribs.GetMaxSize())
        if (size > attribs.GetMaxSize())
            LinkManager::LinkError("Overlay " + name + " exceeds maximum size ");
    if (!attribs.GetSize())
        attribs.SetSize(new LinkExpression(size));
    ObjInt maxSize = base + size - partitionAttribs.GetAddress();
    return maxSize;
}
