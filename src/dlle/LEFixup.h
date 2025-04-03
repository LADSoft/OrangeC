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

#ifndef LEfixup_h
#define LEfixup_h

#include "ObjTypes.h"
#include <fstream>
#include <deque>
#include <map>
#include <memory>

class ObjFile;
class ObjSection;
class ObjExpression;

class LEObject;

class LEFixup
{
  public:
    LEFixup(ObjFile& file, std::deque<LEObject*>& Objects, bool Lx) :
        objects(Objects), pages(0), fixupSize(0), indexTableSize(0), lx(Lx)
    {
        LoadFixups(file);
    }

    void Setup();
    void Write(std::fstream& stream);
    unsigned CreateSections();
    unsigned GetFixupSize() { return fixupSize; }
    unsigned GetIndexTableSize() { return indexTableSize; }
    int SectionOf(ObjExpression* e);

  protected:
    bool IsRel(ObjExpression* e);
    void LoadFixups(ObjFile& file);
    struct Target
    {
        Target() : target(0), section(-1), origSection(0) {}
        Target(ObjInt targ, int sect, int origSect) : target(targ), section(sect), origSection(origSect) {}
        ObjInt target;
        int section;
        int origSection;
    };
    std::map<ObjInt, Target> fixups;
    std::deque<LEObject*>& objects;
    unsigned pages;
    std::unique_ptr<unsigned char[]> fixupTable;
    std::unique_ptr<unsigned char[]> indexTable;
    unsigned fixupSize;
    unsigned indexTableSize;
    bool lx;
};
#endif