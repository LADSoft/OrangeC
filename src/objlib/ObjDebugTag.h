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

#ifndef OBJDEBUGTAG_H
#define OBJDEBUGTAG_H
#include <cstdlib>
#include "ObjTypes.h"

class ObjLineNo;
class ObjSymbol;
class ObjFunction;
class ObjSection;

class ObjDebugTag : public ObjWrapper
{
  public:
    enum eType
    {
        eVar,
        eLineNo,
        eBlockStart,
        eBlockEnd,
        eFunctionStart,
        eFunctionEnd,
        eVirtualFunctionStart,
        eVirtualFunctionEnd
    };
    ObjDebugTag(ObjLineNo* LineNo) : type(eLineNo), lineNo(LineNo){};
    ObjDebugTag(ObjSymbol* Symbol) : type(eVar), symbol(Symbol){};
    ObjDebugTag(ObjSection* Section, bool Start) : type(Start ? eVirtualFunctionStart : eVirtualFunctionEnd), section(Section) {}
    ObjDebugTag(ObjSymbol* Symbol, bool Start) : type(Start ? eFunctionStart : eFunctionEnd), symbol(Symbol) {}
    ObjDebugTag(bool Start) : type(Start ? eBlockStart : eBlockEnd), lineNo(nullptr){};
    virtual ~ObjDebugTag() {}
    eType GetType() { return type; }
    void SetType(eType Type) { type = Type; }
    ObjLineNo* GetLineNo() { return type == eLineNo ? lineNo : nullptr; }
    ObjSymbol* GetSymbol() { return symbol; }
    ObjSection* GetSection() { return section; }

  private:
    enum eType type;
    union
    {
        ObjLineNo* lineNo;
        ObjSymbol* symbol;
        ObjSection* section;
    };
};
#endif