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

#ifndef LINKLIBRARY_H
#define LINKLIBRARY_H

#include "ObjTypes.h"
#include <map>
#include <set>
#include "LibManager.h"
class ObjSymbol;
class ObjFactory;
class ObjFile;

class LinkLibrary
{
  public:
    LinkLibrary(const ObjString& Name, bool CaseSensitive) : name(Name), manager(name, CaseSensitive) {}
    ~LinkLibrary() { Close(); }
    ObjString GetName() { return name; }
    const std::vector<unsigned>& GetSymbol(const ObjString& name) { return manager.Lookup(name); }
    ObjFile* LoadSymbol(ObjInt objNum, ObjFactory* factory)
    {
        loadedModules.insert(objNum);
        return manager.LoadModule(objNum, factory);
    }
    void Close() { manager.Close(); }
    bool IsOpen() { return manager.IsOpen(); }
    bool fail() { return manager.fail(); }
    bool Load() { return manager.LoadLibrary(); }
    bool HasModule(ObjInt objNum) { return loadedModules.find(objNum) != loadedModules.end(); }

  private:
    ObjString name;
    LibManager manager;
    std::set<ObjInt> loadedModules;
};
#endif
