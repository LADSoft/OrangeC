/* Software License Agreement
 * 
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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

#define DEBUG_VERSION 4.0
#include "ObjType.h"

class ObjFactory;
class ObjFile;

#include <map>

class dbgtypes
{
  public:
    dbgtypes(ObjFactory& Factory, ObjFile* FI) : factory(Factory), fi(FI) {}
    ObjType* Put(SimpleType* tp, bool istypedef = false);
    void OutputTypedef(struct SimpleSymbol* sym);

  protected:
    ObjType* Lookup(SimpleType* tp);
    ObjType* BasicType(SimpleType* tp);
    ObjType* TypeName(ObjType* val, const char* nm);
    void StructFields(ObjType::eType sel, ObjType* val, int sz, SimpleSymbol* parent, LIST* hr);
    void EnumFields(ObjType* val, ObjType* base, int sz, LIST* hr);
    ObjType* Function(SimpleType* tp);
    ObjType* ExtendedType(SimpleType* tp);

  private:
    struct typecompare
    {
        bool operator()(const SimpleType* left, const SimpleType* right) const;
    };
    std::map<SimpleType*, ObjType*, typecompare> hash;
    ObjFactory& factory;
    ObjFile* fi;
};
