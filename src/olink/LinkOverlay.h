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

#ifndef LINKOVERLAY_H
#define LINKOVERLAY_H
#include <vector>
#include "LinkAttribs.h"

class LinkRegion;
class LinkPartition;
class LinkTokenizer;
class LinkExpression;
class LinkManager;
class CmdFiles;
class LinkRegionSpecifier;
class ObjFile;

class LinkOverlay
{
    typedef std::vector<std::unique_ptr<LinkRegionSpecifier>> RegionContainer;

  public:
    LinkOverlay(LinkPartition* Parent) : parent(Parent) {}
    ~LinkOverlay();

    ObjString& GetName() { return name; }
    void SetName(const ObjString& Name) { name = Name; }

    LinkAttribs& GetAttribs() { return attribs; }

    LinkPartition* GetParent() { return parent; }
    void SetParent(LinkPartition* Parent) { parent = Parent; }

    typedef RegionContainer::iterator iterator;

    iterator begin() { return regions.begin(); }
    iterator end() { return regions.end(); }

    void Add(LinkRegionSpecifier* region);

    bool ParseOverlaySpec(LinkManager* manager, CmdFiles& files, LinkTokenizer& spec);

    ObjInt PlaceOverlay(LinkManager* manager, LinkAttribs& partitionAttribs, bool completeLink, int overlayNum);

  private:
    bool ParseAssignment(LinkTokenizer& spec);
    bool ParseName(LinkTokenizer& spec);
    bool ParseValue(LinkTokenizer& spec, LinkExpression** rv, bool alreadyassign = false);
    bool ParseAttributes(LinkTokenizer& spec);
    ObjString name;
    LinkPartition* parent;
    RegionContainer regions;
    LinkAttribs attribs;
};
class LinkRegionSpecifier
{
  public:
    LinkRegionSpecifier(LinkRegion* Region) : region(Region) {}
    LinkRegionSpecifier(LinkExpressionSymbol* Symbol) : symbol(Symbol) {}
    ~LinkRegionSpecifier();
    LinkRegion* GetRegion() { return region.get(); }
    LinkExpressionSymbol* GetSymbol() { return symbol.get(); }

  private:
    std::unique_ptr<LinkRegion> region;
    std::unique_ptr<LinkExpressionSymbol> symbol;
};
#endif
