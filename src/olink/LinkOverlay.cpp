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
#include "LinkManager.h"
#include "LinkOverlay.h"
#include "LinkRegion.h"
#include "LinkTokenizer.h"
#include "LinkExpression.h"
#include "CmdFiles.h"

LinkRegionSpecifier::~LinkRegionSpecifier()
{
    delete region;
    delete symbol;
}
LinkOverlay::~LinkOverlay()
{
    for (RegionIterator it = regions.begin(); it != regions.end(); ++it)
    {
        LinkRegionSpecifier *s = *it;
        delete s;
    }
}
bool LinkOverlay::ParseAssignment(LinkTokenizer &spec)
{
    ObjString symName = spec.GetSymbol();
    LinkExpression *value;
    spec.NextToken();
    if (!spec.MustMatch(LinkTokenizer::eAssign))
        return false;
    if (!spec.GetExpression(&value, true))
        return false;
    LinkExpressionSymbol *esym = new LinkExpressionSymbol(symName, value) ;
    if (!LinkExpression::EnterSymbol(esym))
    {
        delete esym;
        LinkManager::LinkError("Symbol " + symName + " redefined");
    }
    else
    {
        LinkRegionSpecifier *lrs = new LinkRegionSpecifier(esym);
        regions.push_back(lrs);
    }
    return spec.MustMatch(LinkTokenizer::eSemi);
}
bool LinkOverlay::ParseName(LinkTokenizer &spec)
{
    if (!spec.Matches(LinkTokenizer::eSymbol))
        return false;
    SetName(spec.GetSymbol());
    spec.NextToken();
    return true;
}
bool LinkOverlay::ParseValue(LinkTokenizer &spec, LinkExpression **rv, bool alreadyassign)
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
bool LinkOverlay::ParseAttributes( LinkTokenizer &spec)
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
bool LinkOverlay::ParseOverlaySpec(LinkManager *manager, CmdFiles &files, LinkTokenizer &spec)
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
            LinkRegion *newRegion = new LinkRegion(this);
            regions.push_back(new LinkRegionSpecifier(newRegion));
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
ObjInt LinkOverlay::PlaceOverlay(LinkManager *manager, LinkAttribs &partitionAttribs, bool completeLink, int overlayNum)
{
    ObjInt size = 0;
    ObjInt base = partitionAttribs.GetAddress();
    ObjInt alignRegion0 = 1;
    for (RegionIterator it = regions.begin(); it != regions.end(); ++it)
    {
        if (!(*it)->GetSymbol())
        {
            alignRegion0= (*it)->GetRegion()->GetAttribs().GetAlign();
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
    for (RegionIterator it = regions.begin(); it != regions.end(); ++it)
    {
        if ((*it)->GetSymbol())
        {
//			if (completeLink)
//				(*it)->GetSymbol()->SetValue(new LinkExpression((*it)->GetSymbol()->GetValue()->Eval(base + size)));
//			else
                (*it)->GetSymbol()->SetValue((*it)->GetSymbol()->GetValue()->Eval(overlayNum, base, size));
        }
        else
        {
            size += (*it)->GetRegion()->PlaceRegion(manager, attribs, base + size);
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
        size += attribs.GetRoundSize() -1;
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
