/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

#ifndef OBJFUNCTION_H
#define OBJFUNCTION_H

#include <vector>
#include "ObjTypes.h"
#include "ObjType.h"
class ObjSymbol;

class ObjFunction : public ObjType
{
    typedef std::vector<ObjType*> ParameterContainer;

  public:
    // clang-format off
    enum eLinkage { eLabel, eC, eStdcall, ePascal, eMethod };
    // clang-format on
    ObjFunction(const ObjString& Name, ObjType* returnType, ObjInt Index) : ObjType(Name, eFunction, returnType, Index), linkage(eLabel) {}
    virtual ~ObjFunction() {}
    void Add(ObjType* Param)
    {
        if (Param)
            parameters.push_back(Param);
    }
    eLinkage GetLinkage() { return linkage; }
    void SetLinkage(eLinkage Linkage) { linkage = Linkage; }
    ObjType* GetReturnType() { return GetBaseType(); }
    void SetReturnType(ObjType* ReturnType) { SetBaseType(ReturnType); }

    typedef ParameterContainer::iterator iterator;
    typedef ParameterContainer::const_iterator const_iterator;

    iterator begin() { return parameters.begin(); }
    iterator end() { return parameters.end(); }

  private:
    enum eLinkage linkage;
    ParameterContainer parameters;
};
#endif