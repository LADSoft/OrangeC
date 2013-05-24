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
#include "LinkManager.h"
#include "LinkMap.h"
#include "LinkAttribs.h"
#include "LinkPartition.h"
#include "LinkOverlay.h"
#include "LinkRegion.h"
#include "ObjExpression.h"
#include "ObjFile.h"
#include "ObjSection.h"
#include "Utils.h"
#include <iomanip>
#include <set>

std::fstream &LinkMap::Address(std::fstream &stream, ObjInt base, ObjInt offset, int group)
{
    switch (mode)
    {
        default:
        case eLinear:
            stream << std::setw(6) << std::setfill('0') << std::hex << base + offset;
            break;
        case eSeg32:
            stream << std::setw(2) << std::setfill('0') << std::hex << group << ":" << std::setw(6) << std::setfill('0') << std::hex << offset ;
            break;
        case eSeg16:
            stream << std::setw(4) << std::setfill('0') << std::hex << (base >> 4) << ":" << std::setw(4) << std::setfill('0') << std::hex << (offset + (base & 15));
            break;
    }
    return stream;
}
ObjInt LinkMap::PublicBase(ObjExpression *exp, int &group)
{	
    ObjExpression *find = exp;
    while (find->GetOperator() == ObjExpression::eAdd || find->GetOperator() == ObjExpression:: eSub || find->GetOperator() == ObjExpression::eDiv)
    {
        find = find->GetLeft();
    }
    if (find->GetOperator() != ObjExpression::eSection)
        Utils::fatal("Invalid fixup");
    group = find->GetSection()->GetIndex() + 1;
    find->GetSection()->SetOffset(new ObjExpression(0)); // this wrecks the link but is done last so it is ok
    return overlays[group-1]->GetAttribs().GetAddress();
}
void LinkMap::ShowAttribs(std::fstream &stream, LinkAttribs &attribs, ObjInt offs, int group)
{
    if (group > 0)
    {
        stream << " addr=" ;
        Address(stream, offs, attribs.GetAddress(), group);
    }
    else
        stream << " addr=" << std::setw(6) << std::setfill('0') << std::hex << attribs.GetAddress();
    if (attribs.GetVirtualOffsetSpecified())
        stream << "(" << std::setw(6) << std::setfill('0') << std::hex << attribs.GetVirtualOffset() << ") ";
    stream << " align=" << std::setw(3) << std::setfill('0') << std::hex << attribs.GetAlign();
    stream << " size=" << std::setw(4) << std::setfill('0') << std::hex << attribs.GetSize();
    stream << " max=" << std::setw(4) << std::setfill('0') << std::hex << attribs.GetMaxSize();
    stream << " round=" << std::setw(3) << std::setfill('0') << std::hex << attribs.GetRoundSize();
    stream << " fill=" << std::setw(2) << std::setfill('0') << std::hex << attribs.GetFill();
    stream << std::endl;
}
void LinkMap::ShowPartitionLine(std::fstream &stream, LinkPartition *partition)
{
    if (partition)
    {
        LinkAttribs &attribs = partition->GetAttribs();
        stream << "Partition: " << partition->GetName();
        ShowAttribs(stream, attribs, 0, -1);
    }
}
void LinkMap::ShowOverlayLine(std::fstream &stream, LinkOverlay *overlay)
{
    if (overlay)
    {
        LinkAttribs &attribs = overlay->GetAttribs();
        stream << "  Overlay: " << overlay->GetName();
        ShowAttribs(stream, attribs, 0, -1);
    }
}
void LinkMap::ShowRegionLine(std::fstream &stream, LinkRegion *region, ObjInt offs, int group)
{
    if (region)
    {
        LinkAttribs &attribs = region->GetAttribs();
        stream << "    Region: " << region->GetName();
        ShowAttribs(stream, attribs, offs, group );
    }
}
void LinkMap::ShowFileLine(std::fstream &stream, LinkRegion::OneSection *data, ObjInt n)
{
    stream << "      File: " << data->file->GetName() << "(" << data->section->GetName() << ") " ;
    stream << "addr=" << std::setw(6) << std::setfill('0') << std::hex << data->section->GetOffset()->Eval(0) /*+ n*/ << " " ;
    stream << "size=" << std::setw(4) << std::setfill('0') << std::hex << data->section->GetSize()->Eval(0) << " " ;
    stream << std::endl;
}
void LinkMap::ShowSymbol(std::fstream &stream, const MapSymbolData &symbol)
{
    Address(stream, symbol.base, symbol.abs - symbol.base, symbol.group);
    if (symbol.sym->GetUsed())
        stream << "   ";
    else
        stream << " X ";
    stream << symbol.sym->GetSymbol()->GetName() << std::setw(1) << std::endl;
}
void LinkMap::NormalSections(std::fstream &stream)
{
    stream << "Partition Map" << std::endl << std::endl;
    for (LinkManager::PartitionIterator it = manager->PartitionBegin(); 
             it != manager->PartitionEnd(); ++it)
    {
        ShowPartitionLine(stream, (*it)->GetPartition());
        if ((*it)->GetPartition())
        {
            for (LinkPartition::OverlayIterator itc = (*it)->GetPartition()->OverlayBegin();
                 itc != (*it)->GetPartition()->OverlayEnd(); ++itc)
            {
                if ((*itc)->GetOverlay())
                {
                    overlays.push_back((*itc)->GetOverlay());
                }
            }
        }
    }
}
void LinkMap::DetailedSections(std::fstream &stream)
{
    stream << "Detailed Section Map" << std::endl << std::endl;
    int group = 1;
    for (LinkManager::PartitionIterator it = manager->PartitionBegin(); 
             it != manager->PartitionEnd(); ++it)
    {
        if ((*it)->GetPartition())
        {
            ShowPartitionLine(stream, (*it)->GetPartition());
            for (LinkPartition::OverlayIterator itc = (*it)->GetPartition()->OverlayBegin();
                 itc != (*it)->GetPartition()->OverlayEnd(); ++itc)
            {
                if ((*itc)->GetOverlay())
                {
                    overlays.push_back((*itc)->GetOverlay());
                    ShowOverlayLine(stream, (*itc)->GetOverlay());
                    for (LinkOverlay::RegionIterator itr = (*itc)->GetOverlay()->RegionBegin();
                             itr != (*itc)->GetOverlay()->RegionEnd(); ++itr)
                    {
                        if ((*itr)->GetRegion())
                        {
                            int n = (*itc)->GetOverlay()->GetAttribs().GetAddress();
                            ShowRegionLine(stream, (*itr)->GetRegion(), n, group);
                            for (LinkRegion::SectionDataIterator it = (*itr)->GetRegion()->NowDataBegin();
                                 it != (*itr)->GetRegion()->NowDataEnd(); ++ it)
                            {
                                for (LinkRegion::OneSectionIterator it1 = (*it)->sections.begin();
                                     it1 != (*it)->sections.end(); ++ it1)
                                {
                                    ShowFileLine (stream, &(*it1), n);
                                }
                            }
                            for (LinkRegion::SectionDataIterator it = (*itr)->GetRegion()->NormalDataBegin();
                                 it != (*itr)->GetRegion()->NormalDataEnd(); ++ it)
                            {
                                for (LinkRegion::OneSectionIterator it1 = (*it)->sections.begin();
                                     it1 != (*it)->sections.end(); ++ it1)
                                {
                                    ShowFileLine (stream, &(*it1), n);
                                }
                            }
                            for (LinkRegion::SectionDataIterator it = (*itr)->GetRegion()->PostponeDataBegin();
                                 it != (*itr)->GetRegion()->PostponeDataEnd(); ++ it)
                            {
                                for (LinkRegion::OneSectionIterator it1 = (*it)->sections.begin();
                                     it1 != (*it)->sections.end(); ++ it1)
                                {
                                    ShowFileLine (stream, &(*it1), n);
                                }
                            }
                        }
                    }
                }
            }
            group++;
        }
    }
}
void LinkMap::Publics(std::fstream &stream)
{
    std::set<MapSymbolData, linkltcomparebyname> byName;
    std::set<MapSymbolData, linkltcomparebyvalue> byValue;
    for (LinkManager::SymbolIterator it = manager->PublicBegin(); it != manager->PublicEnd(); it++)
    {
        int group;
        ObjInt base = PublicBase((*it)->GetSymbol()->GetOffset(), group);
        int ofs = (*it)->GetSymbol()->GetOffset()->Eval(0);
        byName.insert(MapSymbolData((*it), ofs+base, base, group));
        byValue.insert(MapSymbolData((*it), ofs+base, base, group));
    }
    stream << std::endl << "Publics By Name" << std::endl << std::endl;
    for (std::set<MapSymbolData, linkltcomparebyname>::iterator it 
             = byName.begin(); it != byName.end(); it++)
        ShowSymbol(stream, (*it));
    stream << std::endl << "Publics By Value" << std::endl << std::endl;
    for (std::set<MapSymbolData, linkltcomparebyvalue>::iterator it 
             = byValue.begin(); it != byValue.end(); it++)
        ShowSymbol(stream, (*it));
}
void LinkMap::WriteMap()
{
    std::fstream stream(name.c_str(), std::ios_base::out | std::ios_base::trunc);
    if (stream != NULL)
    {
        if (type == eDetailed)
            DetailedSections(stream);
        else
            NormalSections(stream);
        if (type != eNormal)
        {
            Publics(stream);
        }
        stream.close();
    }
}
