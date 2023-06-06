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

#ifndef ResourceId_h
#define ResourceId_h

#include <string>

class RCFile;
class ResFile;

class ResourceId
{
  public:
    ResourceId() : id(-1) {}
    ResourceId(const std::wstring& Name) : name(Name), id(-1) {}
    ResourceId(int Id) : id(Id) {}

    ResourceId(const ResourceId& Old)
    {
        id = Old.id;
        name = Old.name;
    }
    bool operator==(const ResourceId& right) { return id == right.id && name == right.name; }
    bool IsNamed() const { return id == -1; }
    void SetName(const std::wstring& Name) { name = Name; }
    std::wstring GetName() const { return name; }
    void SetId(int Id) { id = Id; }
    int GetId() const { return id; }

    void WriteRes(ResFile& resFile);
    void ReadRC(RCFile& rcFile, bool quoted = false);

  private:
    std::wstring name;
    int id;
};

#endif