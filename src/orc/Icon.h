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

#ifndef Icon_h
#define Icon_h

#include "Resource.h"
#include "ResourceData.h"
#include <deque>
#include <memory>
class RCFile;
class ResFile;
class ResourceData;

class Icon : public Resource
{
  public:
    Icon(const ResourceInfo& info) :
        Resource(eIcon, ResourceId(++nextIconIndex), info), colors(0), planes(0), bits(0), data(nullptr)
    {
    }
    Icon(const ResourceInfo& info, const ResourceId& id) : Resource(eIcon, id, info), colors(0), planes(0), bits(0), data(nullptr)
    {
    }
    virtual ~Icon() {}
    void ReadBin(ResourceData* rd);
    virtual void WriteRes(ResFile& resFile);
    virtual void ReadRC(RCFile& rcFile) {}
    unsigned GetIndex() const { return GetId().GetId(); }
    void SetSize(const Point& Size) { size = Size; }
    Point GetSize() const { return size; }
    void SetColors(const unsigned Colors) { colors = Colors; }
    unsigned GetColors() const { return colors; }
    void SetPlanes(const unsigned Planes) { planes = Planes; }
    unsigned GetPlanes() const { return planes; }
    unsigned GetBytes() const
    {
        if (data)
            return data->GetLen();
        else
            return 0;
    }
    void SetBits(const unsigned Bits) { bits = Bits; }
    unsigned GetBits() const { return bits; }
    void SetData(ResourceData* rdata) { data.reset(rdata); }
    ResourceData* GetData() const { return data.get(); }
    static void Reset() { nextIconIndex = 0; }

  protected:
    std::unique_ptr<ResourceData> data;

  private:
    Point size;
    unsigned colors;
    unsigned planes;
    unsigned bits;
    static int nextIconIndex;
};
class GroupIcon : public Resource
{
  public:
    GroupIcon(const ResourceId& Id, const ResourceInfo& info) : Resource(eGroupIcon, Id, info) {}
    virtual ~GroupIcon() {}
    virtual void WriteRes(ResFile& resFile);
    virtual void ReadRC(RCFile& rcFile);

    void Add(Icon* icon) { icons.push_back(icon); }

    typedef std::deque<Icon*>::iterator iterator;
    iterator begin() { return icons.begin(); }
    iterator end() { return icons.end(); }

  private:
    std::deque<::Icon*> icons;
};

#endif