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

#ifndef ResidentNameTable_h
#define ResidentNameTable_h

#include <string>
#include <fstream>
#include <memory>

class ResidentNameTable
{
  public:
    ResidentNameTable(const std::string& FileName) : fileName(FileName), entryOffset(0), size(0) {}
    virtual ~ResidentNameTable() {}
    unsigned GetEntrySize() { return entryOffset; }
    unsigned GetSize() { return size; }
    void Setup();
    void Write(std::fstream& stream);

  private:
    std::string fileName;
    std::unique_ptr<char[]> data;
    int entryOffset;
    int size;
};
#endif