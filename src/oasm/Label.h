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

#ifndef Label_H
#define Label_H

#include <string>
#include "AsmExpr.h"
class ObjSymbol;
class ObjSection;
class Section;

class Label
{
  public:
    Label(std::string& Name, int Label, int Sect) :
        name(Name), label(Label), sect(Sect), publc(false), extrn(false), sym(nullptr), objSection(nullptr)
    {
    }
    int GetLabel() { return label; }
    std::string& GetName() { return name; }
    std::shared_ptr<AsmExprNode> GetOffset() { return node; }
    void SetOffset(std::shared_ptr<AsmExprNode> Offset) { node = std::move(Offset); }
    void SetOffset(int offs) { SetOffset(std::make_shared<AsmExprNode>(offs)); }
    int GetSect() { return sect; }
    void SetSect(int Sect) { sect = Sect; }
    void SetObjectSection(ObjSection* os) { objSection = os; }
    ObjSection* GetObjectSection() { return objSection; }
    bool IsPublic() { return publc; }
    void SetPublic(bool Publc) { publc = Publc; }
    bool IsExtern() { return extrn; }
    void SetExtern(bool Extrn) { extrn = Extrn; }
    void SetObjSymbol(ObjSymbol* Sym) { sym = Sym; }
    ObjSymbol* GetObjSymbol() { return sym; }

  private:
    std::string name;
    std::shared_ptr<AsmExprNode> node;
    int sect;
    int label;
    bool publc;
    bool extrn;
    ObjSymbol* sym;
    ObjSection* objSection;
};
#endif