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

#include "ObjFile.h"
#include "ObjSection.h"
#include "ObjSymbol.h"

void ObjFile::ResolveSymbols(ObjFactory* Factory)
{
    for (auto it = SectionBegin(); it != SectionEnd(); ++it)
        (*it)->ResolveSymbols(Factory);
}
void ObjFile::Add(ObjSymbol* Symbol)
{
    switch (Symbol->GetType())
    {
        case ObjSymbol::ePublic:
            publics.push_back(Symbol);
            break;
        case ObjSymbol::eExternal:
            externals.push_back(Symbol);
            break;
        case ObjSymbol::eLocal:
            locals.push_back(Symbol);
            break;
        case ObjSymbol::eAuto:
            autos.push_back(Symbol);
            break;
        case ObjSymbol::eReg:
            regs.push_back(Symbol);
            break;
        case ObjSymbol::eImport:
            imports.push_back(Symbol);
            break;
        case ObjSymbol::eExport:
            exports.push_back(Symbol);
            break;
        case ObjSymbol::eDefinition:
            definitions.push_back(Symbol);
            break;
        default:
            break;
    }
}
ObjSection* ObjFile::FindSection(const ObjString& Name)
{
    for (auto sect : sections)
        if (sect->GetName() == Name)
            return sect;
    return nullptr;
}
