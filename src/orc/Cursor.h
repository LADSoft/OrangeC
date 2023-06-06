/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#ifndef Cursor_h
#define Cursor_h

#include "Resource.h"
#include "Icon.h"

#include <deque>

class RCFile;
class ResFile;
class ResourceData;

class Cursor : public Icon
{
  public:
    Cursor(const ResourceInfo& info) : Icon(info, ResourceId(++nextCursorIndex)) { SetType(eCursor); }
    virtual ~Cursor() {}
    virtual void WriteRes(ResFile& resFile);
    bool ReadBin(ResourceData* rd);

    void SetHotspot(Point& hs) { hotspot = hs; }
    Point GetHotspot() const { return hotspot; }
    static void Reset() { nextCursorIndex = 0; }

  private:
    Point hotspot;
    static int nextCursorIndex;
};
class GroupCursor : public Resource
{
  public:
    GroupCursor(const ResourceId& Id, const ResourceInfo& info) : Resource(eGroupCursor, Id, info) {}
    virtual ~GroupCursor() {}
    virtual void WriteRes(ResFile& resFile);
    virtual void ReadRC(RCFile& rcFile);

    void Add(Cursor* Cursor) { cursors.push_back(Cursor); }
    typedef std::deque<Cursor*>::iterator iterator;
    iterator begin() { return cursors.begin(); }
    iterator end() { return cursors.end(); }

  private:
    std::deque<Cursor*> cursors;
};

#endif