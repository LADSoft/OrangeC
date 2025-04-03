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

#ifndef OBJSOURCEFILE_H
#define OBJSOURCEFILE_H

#include <ctime>
#include <cstdlib>
#include <cstring>
#include "ObjIndexManager.h"

class ObjSourceFile : public ObjWrapper
{
  public:
    ObjSourceFile(const ObjString& Name, ObjInt Index) : name(Name), index(Index)
    {
        memset(&fileTime, 0, sizeof(fileTime));
        RetrieveFileTime(name);
    }
    virtual ~ObjSourceFile() {}
    ObjString& GetName() { return name; }
    void SetName(ObjString& Name) { name = Name; }
    ObjInt GetIndex() { return index; }
    void SetIndex(ObjInt Index) { index = Index; }
    std::tm GetFileTime() { return fileTime; }
    void SetFileTime(std::tm FileTime) { fileTime = FileTime; }
    void RetrieveFileTime(const ObjString& name);

  private:
    ObjString name;
    std::tm fileTime;
    ObjInt index;
};
#endif
