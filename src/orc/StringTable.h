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

#ifndef StringTable_h
#define StringTable_h

#include "Resource.h"
#include <string>
#include <map>

class RCFile;
class ResFile;

class StringTable : public Resource
{
  public:
    StringTable(const ResourceInfo& info);
    virtual ~StringTable() {}
    virtual void WriteRes(ResFile& resFile);
    virtual void ReadRC(RCFile& rcFile);

    static void Reset() { stringTables.clear(); }

  protected:
    StringTable(int Id, const ResourceInfo& info);
    void Add(int id, const std::wstring& val);
    static StringTable* Lookup(ResFile& resFile, int id);

  private:
    int tblNum;
    std::wstring strings[16];
    ResourceInfo loadedInfo;
    static std::map<int, StringTable*> stringTables;
    static ResourceInfo currentInfo;
};
#endif
