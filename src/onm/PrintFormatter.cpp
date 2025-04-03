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

#include "PrintFormatter.h"
#include "SymbolTable.h"
#include <sstream>
#include <iostream>
#include <iomanip>
std::string PrintFormatter::GetOffset(Symbol* sym, int radix, ObjInt digits)
{
    std::stringstream strm;
    strm << std::setw(digits + 1) << std::setbase(radix) << sym->offset;
    return strm.str();
}

void PrintFormatter::Print(SymbolTable& symbols)
{
    int nameSize = 0;
    int symbolSize = 0;
    ObjInt maxOffs = 0;
    for (auto s : symbols.symbols)
    {
        int n = s->fileName.size();
        int n1 = (demangle ? s->displayName : s->symbolName).size();
        if (n > nameSize)
            nameSize = n;
        if (n1 > symbolSize)
            symbolSize = n;
        if (s->offset > maxOffs)
            maxOffs = s->offset;
    }
    int rad = 16;
    if (!radix.empty())
    {
        if (radix[0] == 'd')
            rad = 10;
        else if (radix[0] == 'o')
            rad = 8;
    }
    double aa = (int)(maxOffs / rad / rad / rad / rad);
    maxOffs = 4;
    while (aa >= rad)
    {
        maxOffs++;
        aa /= rad;
    }
    if (filePerLine)
    {
        std::string lastFile;
        for (auto s : symbols.symbols)
        {
            std::cout << GetOffset(s, rad, maxOffs) << " " << GetType(s) << " " << std::setw(symbolSize + 1)
                      << (demangle ? s->displayName : s->symbolName) << " " << s->fileName << std::endl;
        }
    }
    else
    {
        std::string lastFile;
        for (auto s : symbols.symbols)
        {
            if (lastFile != s->fileName)
            {
                lastFile = s->fileName;
                std::cout << s->fileName << ":" << std::endl;
            }
            std::cout << GetOffset(s, rad, maxOffs) << " " << GetType(s) << " " << (demangle ? s->displayName : s->symbolName)
                      << std::endl;
        }
    }
}
