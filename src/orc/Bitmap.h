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

#ifndef Bitmap_h
#define Bitmap_h

#include "Resource.h"
#include <memory>

class RCFile;
class ResFile;
class ResourceData;

class Bitmap : public Resource
{
  public:
    Bitmap(const ResourceId& Id, const ResourceInfo& info) : Resource(eBitmap, Id, info), data(nullptr) {}
    virtual ~Bitmap();
    virtual void WriteRes(ResFile& resFile);
    virtual void ReadRC(RCFile& rcFile);
    void SetData(ResourceData* rdata);
    ResourceData* GetData() const { return data.get(); }

  private:
    std::unique_ptr<ResourceData> data;
};
#endif