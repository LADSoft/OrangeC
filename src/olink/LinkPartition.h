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

#ifndef LINKPARTITION_H
#define LINKPARTITION_H

#include <vector>
#include <set>
#include "LinkAttribs.h"

class LinkOverlay;
class LinkManager;
class LinkTokenizer;
class LinkExpression;
class CmdFiles;
class LinkOverlaySpecifier;
class ObjFile;

class LinkPartition
{
    typedef std::vector<std::unique_ptr<LinkOverlaySpecifier>> OverlayContainer;

  public:
    LinkPartition(LinkManager* Parent) : name(""), parent(Parent) {}
    ~LinkPartition();

    ObjString& GetName() { return name; }
    void SetName(const ObjString& Name) { name = Name; }

    LinkAttribs& GetAttribs() { return attribs; }

    typedef OverlayContainer::iterator iterator;

    iterator begin() { return overlays.begin(); }
    iterator end() { return overlays.end(); }

    bool CreateSeparateRegions(LinkManager* manager, CmdFiles& files, LinkTokenizer& spec);
    bool ParsePartitionSpec(LinkManager* manager, CmdFiles& files, LinkTokenizer& spec);

    void Add(LinkOverlaySpecifier* ov);
    ObjInt PlacePartition(LinkManager* manager, ObjInt bottom, bool completeLink, int& overlayNum);

  private:
    bool ParseValue(LinkTokenizer& spec, LinkExpression** rv, bool alreadyassign = false);
    bool ParseAttributes(LinkTokenizer& spec);
    bool ParseName(LinkTokenizer& spec);
    bool ParseOverlays(LinkManager* manager, CmdFiles& files, LinkTokenizer& spec);
    bool ParseAssignment(LinkTokenizer& spec);
    ObjString name;
    OverlayContainer overlays;
    LinkAttribs attribs;
    LinkManager* parent;
};
class LinkPartitionSpecifier
{
  public:
    LinkPartitionSpecifier(LinkPartition* Partition) : partition(Partition) {}
    LinkPartitionSpecifier(LinkExpressionSymbol* Symbol) : symbol(Symbol) {}
    ~LinkPartitionSpecifier() {}
    LinkPartition* GetPartition() { return partition.get(); }
    LinkExpressionSymbol* GetSymbol()
    {
        return symbol.get();
        ;
    }

  private:
    std::unique_ptr<LinkPartition> partition;
    std::unique_ptr<LinkExpressionSymbol> symbol;
};
class LinkOverlaySpecifier
{
  public:
    LinkOverlaySpecifier(LinkOverlay* Overlay) : overlay(Overlay) {}
    LinkOverlaySpecifier(LinkExpressionSymbol* Symbol) : symbol(Symbol) {}
    ~LinkOverlaySpecifier();
    LinkOverlay* GetOverlay() { return overlay.get(); }
    LinkExpressionSymbol* GetSymbol() { return symbol.get(); }

  private:
    std::unique_ptr<LinkOverlay> overlay;
    std::unique_ptr<LinkExpressionSymbol> symbol;
};
#endif
