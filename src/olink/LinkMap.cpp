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

#include "LinkManager.h"
#include "LinkMap.h"
#include "LinkAttribs.h"
#include "LinkPartition.h"
#include "LinkOverlay.h"
#include "LinkRegion.h"
#include "ObjExpression.h"
#include "ObjFile.h"
#include "ObjSection.h"
#include "ObjSymbol.h"
#include "Utils.h"
#include <iomanip>
#include <set>
#include <iostream>
#include <fstream>

std::fstream& LinkMap::Address(std::fstream& stream, ObjInt base, ObjInt offset, int group)
{
    std::ios_base::fmtflags f(stream.flags());
    switch (mode)
    {
        default:
        case eLinear:
            stream << std::setw(6) << std::setfill('0') << std::hex << base + offset;
            break;
        case eSeg32:
            stream << std::setw(2) << std::setfill('0') << std::hex << group << ":" << std::setw(6) << std::setfill('0') << std::hex
                   << offset;
            break;
        case eSeg16:
            stream << std::setw(4) << std::setfill('0') << std::hex << (base >> 4) << ":" << std::setw(4) << std::setfill('0')
                   << std::hex << (offset + (base & 15));
            break;
    }
    stream.flags(f);
    return stream;
}
ObjInt LinkMap::PublicBase(ObjExpression* exp, int& group)
{
    ObjExpression* find = exp;
    while (find->GetOperator() == ObjExpression::eAdd || find->GetOperator() == ObjExpression::eSub ||
           find->GetOperator() == ObjExpression::eDiv)
    {
        find = find->GetLeft();
    }
    if (find->GetOperator() != ObjExpression::eSection)
        Utils::Fatal("Invalid fixup");
    group = find->GetSection()->GetIndex() + 1;
    find->GetSection()->SetOffset(new ObjExpression(0));  // this wrecks the link but is done last so it is ok
    return overlays[group - 1]->GetAttribs().GetAddress();
}
void LinkMap::ShowAttribs(std::fstream& stream, LinkAttribs& attribs, ObjInt offs, int group)
{
    if (group > 0)
    {
        stream << " addr=";
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
void LinkMap::ShowPartitionLine(std::fstream& stream, LinkPartition* partition)
{
    if (partition)
    {
        LinkAttribs& attribs = partition->GetAttribs();
        stream << "Partition: " << partition->GetName();
        ShowAttribs(stream, attribs, 0, -1);
    }
}
void LinkMap::ShowOverlayLine(std::fstream& stream, LinkOverlay* overlay)
{
    if (overlay)
    {
        LinkAttribs& attribs = overlay->GetAttribs();
        stream << "  Overlay: " << overlay->GetName();
        ShowAttribs(stream, attribs, 0, -1);
    }
}
void LinkMap::ShowRegionLine(std::fstream& stream, LinkRegion* region, ObjInt offs, int group)
{
    if (region)
    {
        LinkAttribs& attribs = region->GetAttribs();
        stream << "    Region: " << region->GetName();
        ShowAttribs(stream, attribs, offs, group);
    }
}
void LinkMap::ShowFileLine(std::fstream& stream, LinkRegion::OneSection* data, ObjInt n)
{
    ObjString sectionName = data->section->GetName();
    if (sectionName.substr(0, 4) == "vsc@" && sectionName.substr(0, 7) != "vsc@$xt" && sectionName.substr(0, 7) != "vsc@$xc" &&
        sectionName.find("_$vt") == std::string::npos)
        sectionName = ObjSymbol(sectionName.c_str() + 3, ObjSymbol::eGlobal, 0).GetDisplayName();
    stream << "      File: " << data->file->GetName() << "(" << sectionName << ") ";
    stream << "addr=" << std::setw(6) << std::setfill('0') << std::hex << data->section->GetOffset()->Eval(0) /*+ n*/ << " ";
    stream << "size=" << std::setw(4) << std::setfill('0') << std::hex << data->section->GetSize()->Eval(0) << " ";
    stream << std::endl;
}
void LinkMap::ShowSymbol(std::fstream& stream, const MapSymbolData& symbol)
{
    Address(stream, symbol.base, symbol.abs - symbol.base, symbol.group);
    if (symbol.used)
        stream << "   ";
    else
        stream << " X ";
    stream << symbol.displayName << std::setw(1) << std::endl;
}
void LinkMap::NormalSections(std::fstream& stream)
{
    stream << "Partition Map" << std::endl << std::endl;
    for (auto it = manager->PartitionBegin(); it != manager->PartitionEnd(); ++it)
    {
        ShowPartitionLine(stream, (*it)->GetPartition());
        if ((*it)->GetPartition())
        {
            for (auto&& overlay : *(*it)->GetPartition())
            {
                if (overlay->GetOverlay())
                {
                    overlays.push_back(overlay->GetOverlay());
                }
            }
        }
    }
}
void LinkMap::DetailedSections(std::fstream& stream)
{
    stream << "Detailed Section Map" << std::endl << std::endl;
    int group = 1;
    for (auto it = manager->PartitionBegin(); it != manager->PartitionEnd(); ++it)
    {
        if ((*it)->GetPartition())
        {
            ShowPartitionLine(stream, (*it)->GetPartition());
            for (auto&& overlay : *(*it)->GetPartition())
            {
                if (overlay->GetOverlay())
                {
                    overlays.push_back(overlay->GetOverlay());
                    ShowOverlayLine(stream, overlay->GetOverlay());
                    for (auto&& region : *overlay->GetOverlay())
                    {
                        if (region->GetRegion())
                        {
                            int n = overlay->GetOverlay()->GetAttribs().GetAddress();
                            ShowRegionLine(stream, region->GetRegion(), n, group);
                            for (auto it = region->GetRegion()->NowDataBegin(); it != region->GetRegion()->NowDataEnd(); ++it)
                            {
                                for (auto sect : (*it)->sections)
                                {
                                    ShowFileLine(stream, &sect, n);
                                }
                            }
                            for (auto it = region->GetRegion()->NormalDataBegin(); it != region->GetRegion()->NormalDataEnd(); ++it)
                            {
                                for (auto sect : (*it)->sections)
                                {
                                    ShowFileLine(stream, &sect, n);
                                }
                            }
                            for (auto it = region->GetRegion()->PostponeDataBegin(); it != region->GetRegion()->PostponeDataEnd();
                                 ++it)
                            {
                                for (auto sect : (*it)->sections)
                                {
                                    ShowFileLine(stream, &sect, n);
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
void LinkMap::Publics(std::fstream& stream)
{
    std::set<MapSymbolData, linkltcomparebyname> byName;
    std::set<MapSymbolData, linkltcomparebyvalue> byValue;
    for (auto it = manager->PublicBegin(); it != manager->PublicEnd(); it++)
    {
        int group;
        ObjInt base = PublicBase((*it)->GetSymbol()->GetOffset(), group);
        int ofs = (*it)->GetSymbol()->GetOffset()->Eval(0);
        byName.insert(MapSymbolData((*it)->GetSymbol()->GetDisplayName(), (*it)->GetUsed(), ofs + base, base, group));
        byValue.insert(MapSymbolData((*it)->GetSymbol()->GetDisplayName(), (*it)->GetUsed(), ofs + base, base, group));
    }
    int group = 1;
    for (auto it = manager->PartitionBegin(); it != manager->PartitionEnd(); ++it)
    {
        if ((*it)->GetPartition())
        {
            for (auto&& overlay : *(*it)->GetPartition())
            {
                if (overlay->GetOverlay())
                {
                    ObjInt base = overlay->GetOverlay()->GetAttribs().GetAddress();
                    for (auto&& region : *overlay->GetOverlay())
                    {
                        if (region->GetRegion())
                        {
                            if (region->GetRegion()->GetName() == " vsc* " || region->GetRegion()->GetName() == " vsd* " ||
                                region->GetRegion()->GetName() == " vsb* " || region->GetRegion()->GetName() == " vss* " ||
                                region->GetRegion()->GetName() == " vsr* ")
                            {
                                int base = overlay->GetOverlay()->GetAttribs().GetAddress();
                                for (auto it = region->GetRegion()->NowDataBegin(); it != region->GetRegion()->NowDataEnd(); ++it)
                                {
                                    for (auto sect : (*it)->sections)
                                    {
                                        ObjInt ofs = sect.section->GetOffset()->GetValue();
                                        ObjString sectionName = sect.section->GetName();
                                        if (sectionName.substr(0, 4) == "vsc@" && sectionName.substr(0, 7) != "vsc@$xt" &&
                                            sectionName.substr(0, 7) != "vsc@$xc" && sectionName.find("_$vt") == std::string::npos)
                                            sectionName =
                                                ObjSymbol(sectionName.c_str() + 3, ObjSymbol::eGlobal, 0).GetDisplayName();
                                        byName.insert(MapSymbolData(sectionName, true, ofs + base, base, group));
                                        byValue.insert(MapSymbolData(std::move(sectionName), true, ofs + base, base, group));
                                    }
                                }
                                for (auto it = region->GetRegion()->NormalDataBegin(); it != region->GetRegion()->NormalDataEnd();
                                     ++it)
                                {
                                    for (auto sect : (*it)->sections)
                                    {
                                        ObjInt ofs = sect.section->GetOffset()->GetValue();
                                        ObjString sectionName = sect.section->GetName();
                                        if (sectionName.substr(0, 4) == "vsc@" && sectionName.substr(0, 7) != "vsc@$xt" &&
                                            sectionName.substr(0, 7) != "vsc@$xc" && sectionName.find("_$vt") == std::string::npos)
                                            sectionName =
                                                ObjSymbol(sectionName.c_str() + 3, ObjSymbol::eGlobal, 0).GetDisplayName();
                                        byName.insert(MapSymbolData(sectionName, true, ofs + base, base, group));
                                        byValue.insert(MapSymbolData(std::move(sectionName), true, ofs + base, base, group));
                                    }
                                }
                                for (auto it = region->GetRegion()->PostponeDataBegin();
                                     it != region->GetRegion()->PostponeDataEnd(); ++it)
                                {
                                    for (auto sect : (*it)->sections)
                                    {
                                        ObjInt ofs = sect.section->GetOffset()->GetValue();
                                        ObjString sectionName = sect.section->GetName();
                                        if (sectionName.substr(0, 4) == "vsc@" && sectionName.substr(0, 7) != "vsc@$xt" &&
                                            sectionName.substr(0, 7) != "vsc@$xc" && sectionName.find("_$vt") == std::string::npos)
                                            sectionName =
                                                ObjSymbol(sectionName.c_str() + 3, ObjSymbol::eGlobal, 0).GetDisplayName();
                                        byName.insert(MapSymbolData(sectionName, true, ofs + base, base, group));
                                        byValue.insert(MapSymbolData(std::move(sectionName), true, ofs + base, base, group));
                                    }
                                }
                            }
                        }
                    }
                }
            }
            group++;
        }
    }
    stream << std::endl << "Publics By Name" << std::endl << std::endl;
    for (const auto& sym : byName)
        ShowSymbol(stream, sym);
    stream << std::endl << "Publics By Value" << std::endl << std::endl;
    for (const auto& sym : byValue)
        ShowSymbol(stream, sym);
}
void LinkMap::WriteMap()
{
    std::fstream stream(name, std::ios::out | std::ios::trunc);
    if (!stream.fail())
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
