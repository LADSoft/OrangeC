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

#ifndef OBJBROWSEINFO_H
#define OBJBROWSEINFO_H

#include "ObjTypes.h"

class ObjLineNo;
class ObjMemory;

class ObjBrowseInfo : public ObjWrapper
{
  public:
    // clang-format off
    enum eType { eDefine, eVariable, eFileStart, eFuncStart, eFuncEnd, eBlockStart, 
                 eBlockEnd, eTypePrototype, eUsage  };
    enum eQual { eGlobal, eStatic, eExternal, eLocal, ePrototype, eTypeval };
    // clang-format on
    ObjBrowseInfo(eType Type, eQual Qual, ObjLineNo* Line, ObjInt CharPos, const ObjString& Data) :
        type(Type), qual(Qual), line(Line), charpos(CharPos), data(Data)
    {
    }
    virtual ~ObjBrowseInfo() {}

    ObjLineNo* GetLine() { return line; }
    void SetLine(ObjLineNo* Line) { line = Line; }
    eType GetType() { return type; }
    eQual GetQual() { return qual; }
    void SetType(eType Type) { type = Type; }
    ObjString& GetData() { return data; }
    void SetData(ObjString& Data) { data = Data; }
    ObjLineNo* GetLineNo() { return line; }
    void SetLineNo(ObjLineNo* Line) { line = Line; }
    ObjInt GetCharPos() { return charpos; }
    void SetCharPos(ObjInt CharPos) { charpos = CharPos; }

  private:
    ObjLineNo* line;
    ObjInt charpos;
    eType type;
    eQual qual;
    ObjString data;
};
#endif  // OBJBROWSEINFO_H
