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

#ifndef LEObject_H
#define LEObject_H

#include <string>
#include <set>
#include "ObjTypes.h"
#include <memory>
class ObjSection;
class ObjFile;
class ObjExpression;
inline ObjInt ObjectAlign(ObjInt alignment, ObjInt value) { return (value + alignment - 1) & ~(alignment - 1); }

class LEObject
{
  public:
    enum
    {
        HeaderSize = 6 * 4
    };

    LEObject(ObjSection* Sect) : sect(Sect), pageOffs(0) { InitFlags(); }
    virtual ~LEObject() {}
    void Setup(unsigned& offs);
    void InitFlags();
    unsigned GetSize() { return size; }
    unsigned GetInitSize() { return initSize; }
    void SetSize(unsigned Size) { initSize = size = Size; }
    void SetInitSize(unsigned Size) { initSize = size; }
    unsigned GetAddr() { return base_addr; }
    void SetAddr(unsigned addr) { base_addr = addr; }
    unsigned GetFlags() { return flags; }
    void SetFlags(unsigned Flags) { flags = Flags; }
    unsigned GetPageOffs() { return pageOffs; }
    void WriteHeader(std::fstream& stream);
    void Write(std::fstream& stream);
    static void SetFile(ObjFile* File);

  protected:
    bool IsRel(ObjExpression* e);
    unsigned pageOffs;
    unsigned size;
    unsigned initSize;
    unsigned base_addr;
    unsigned flags;
    std::unique_ptr<unsigned char[]> data;
    std::string name;
    static ObjFile* file;

  private:
    ObjSection* sect;
};
#endif