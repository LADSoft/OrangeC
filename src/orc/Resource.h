/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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
 */

#ifndef Resource_h
#define Resource_h

#include "ResourceId.h"
#include "ResourceInfo.h"
#include <fstream>
#include <deque>

class RCFile;
class ResFile;

struct Point
{
    Point() : x(0), y(0) {}
    Point(int X, int Y) { x = X, y = Y; }
    int x;
    int y;
    void WriteRes(ResFile& resFile);
};

class Resource
{
  public:
    enum ResourceType
    {
        eFileHeader,
        eCursor,
        eBitmap,
        eIcon,
        eMenu,
        eDialog,
        eString,
        eFontDirectory,
        eFont,
        eAccelerators,
        eRCData,
        eMessageTable,
        eGroupCursor,
        eGroupIcon = 14,
        eVersion = 16,
        eDlgInclude,
        ePlugPlay,
        eVXD,
        eAnimatedCursor = 21,
        eAnimatedIcon
    };
    Resource(ResourceType ResType, const ResourceId& Id, const ResourceInfo& info) : type(ResType), id(Id), resInfo(info) {}
    Resource(const ResourceId& ResType, const ResourceId& Id, const ResourceInfo& info) : type(ResType), id(Id), resInfo(info) {}
    virtual ~Resource() {}

    virtual void WriteRes(ResFile& resFile);
    virtual void ReadRC(RCFile& rcFile) {}

    void SetResInfo(const ResourceInfo& info) { resInfo = info; }
    ResourceInfo GetResInfo() const { return resInfo; }
    ResourceId GetType() const { return type; }
    void SetType(int Type) { type = ResourceId(Type); }
    ResourceId GetId() const { return id; }

  protected:
    ResourceInfo resInfo;
    ResourceId type;
    ResourceId id;
};

#endif