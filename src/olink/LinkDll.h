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

#ifndef LINKDLL_H
#define LINKDLL_H

#include "ObjTypes.h"
#include "LinkLibrary.h"
#include <memory>

class LinkDll
{
  public:
    LinkDll(const ObjString& Name, const ObjString& LibPath, bool CaseSensitive) :
        name(Name), libPath(LibPath), caseSensitive(CaseSensitive)
    {
        LoadDll();
    }
    ~LinkDll() {}
    ObjString GetName() { return name; }
    bool IsDll() { return isDll; }
    bool MatchesArchitecture() { return matchesArchitecture; }
    std::unique_ptr<LinkLibrary> LoadLibrary(bool isstdcall);

  protected:
    void LoadDll();

  private:
    ObjString name;
    ObjString libPath;
    bool isDll = false;
    bool matchesArchitecture = false;
    bool caseSensitive;
};
#endif
