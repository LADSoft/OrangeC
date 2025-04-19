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

#ifndef LINKMAP_H
#define LINKMAP_H

#include "ObjTypes.h"
#include "LinkRegion.h"
#include <fstream>

class LinkManager;
class LinkPartition;
class LinkOverlay;
class LinkRegion;
class LinkSymbolData;
class LinkAttribs;

class LinkMap
{
  public:
    enum eMapType
    {
        eNormal,
        ePublic,
        eDetailed
    };
    enum eMapMode
    {
        eLinear,
        eSeg32,
        eSeg16
    };
    LinkMap(eMapType Type, eMapMode MapMode, const ObjString& Name, LinkManager* Manager) :
        type(Type), name(Name), manager(Manager), mode(MapMode)
    {
    }
    ~LinkMap() {}
    void WriteMap();

  private:
    struct MapSymbolData
    {
        MapSymbolData(ObjString DisplayName, bool Used, ObjInt Abs, ObjInt Base, int Group) :
            displayName(std::move(DisplayName)), used(Used), base(Base), abs(Abs), group(Group)
        {
        }
        ObjString displayName;
        ObjInt base;
        ObjInt abs;
        int group;
        bool used;
    };
    std::fstream& Address(std::fstream& stream, ObjInt base, ObjInt offset, ObjInt group);
    ObjInt PublicBase(ObjExpression* exp, int& group);
    void NormalSections(std::fstream& stream);
    void DetailedSections(std::fstream& stream);
    void Publics(std::fstream& stream);
    void ShowPartitionLine(std::fstream& stream, LinkPartition* partition);
    void ShowOverlayLine(std::fstream& stream, LinkOverlay* overlay);
    void ShowRegionLine(std::fstream& stream, LinkRegion* region, ObjInt offs, int group);
    void ShowFileLine(std::fstream& stream, LinkRegion::OneSection* data, ObjInt offs);
    void ShowSymbol(std::fstream& stream, const MapSymbolData& symbol);
    void ShowAttribs(std::fstream& stream, LinkAttribs& attribs, ObjInt offs, int group);
    eMapType type;
    eMapMode mode;
    ObjString name;
    LinkManager* manager;
    struct linkltcomparebyname
    {
        bool operator()(const MapSymbolData& left, const MapSymbolData& right) const
        {
            return left.displayName < right.displayName;
        }
    };
    struct linkltcomparebyvalue
    {
        bool operator()(const MapSymbolData& left, const MapSymbolData& right) const { return left.abs < right.abs; }
    };
    std::vector<LinkOverlay*> overlays;
};
#endif
