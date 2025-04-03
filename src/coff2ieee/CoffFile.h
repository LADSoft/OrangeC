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

#ifndef COFFFILE_H
#define COFFFILE_H

#include "CoffHeader.h"
#include "ObjFile.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
class CoffFile
{
  public:
    CoffFile(std::string Name) : name(Name), inputFile(nullptr), sections(nullptr), symbols(nullptr), strings(nullptr), libOffset(0)
    {
        memset(&header, 0, sizeof(header));
    }
    CoffFile(std::fstream* in, ObjInt offset) :
        name(""), inputFile(in), sections(nullptr), symbols(nullptr), strings(nullptr), libOffset(offset)
    {
        memset(&header, 0, sizeof(header));
    }
    virtual ~CoffFile();

    bool Load();
    ObjFile* ConvertToObject(std::string fileName, ObjFactory& factory);

  protected:
    bool ScanSymbols();
    bool AddComdefs();
    std::string GetSectionName(int sect);
    ObjInt GetSectionQualifiers(int sect);

  private:
    CoffHeader header;

    std::unique_ptr<CoffSection[]> sections;

    std::vector<CoffSymbol*> sectionSymbols;
    std::vector<std::unique_ptr<CoffReloc[]>> relocs;

    std::unique_ptr<CoffSymbol[]> symbols;

    std::unique_ptr<char[]> strings;

    std::string name;

    std::unique_ptr<std::fstream> inputFile;
    unsigned libOffset;
};
#endif
