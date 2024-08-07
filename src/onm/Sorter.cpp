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

#include "Sorter.h"
#include "SymbolTable.h"
#include <vector>
#include <algorithm>
bool Sorter::SortByName(Symbol* left, Symbol* right)
{
    if (left->fileName < right->fileName)
        return true;
    else if (left->fileName == right->fileName)
        return left->symbolName < right->symbolName;
    return false;
}
bool Sorter::SortByAddress(Symbol* left, Symbol* right)
{
    if (left->fileName < right->fileName)
        return true;
    else if (left->fileName == right->fileName)
        return left->offset < right->offset;
    return false;
}
void Sorter::Sort(SymbolTable& table)
{
    if (!noSort)
    {
        if (byAddress)
        {
            std::sort(table.symbols.begin(), table.symbols.end(), &SortByAddress);
        }
        else
        {
            std::sort(table.symbols.begin(), table.symbols.end(), &SortByName);
        }
        if (reverseSort)
        {
            std::reverse(table.symbols.begin(), table.symbols.end());
        }
    }
}