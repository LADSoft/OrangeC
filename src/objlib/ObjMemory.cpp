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

#include "ObjExpression.h"
#include "ObjSymbol.h"
#include "ObjSection.h"
#include "ObjMemory.h"
#include "ObjDebugTag.h"
#include "ObjFactory.h"

ObjMemory::~ObjMemory() {}
void ObjMemory::SetData(ObjByte* Data, ObjInt Size)
{
    data = std::make_unique<ObjByte[]>(Size);
    memcpy(data.get(), Data, Size);
    size = Size;
    fixup = nullptr;
}
void ObjMemory::SetData(ObjExpression* Data, ObjInt Size)
{
    fixup = Data;
    size = Size;
    if (data)
    {
        data = nullptr;
    }
}
void ObjMemoryManager::ResolveSymbols(ObjFactory* Factory, ObjSection* Section)
{
    int offset = 0;
    for (auto mem : *this)
    {
        if (mem->HasDebugTags())
        {
            for (auto tag : *mem)
            {
                if (tag->GetType() == ObjDebugTag::eVar)
                {
                    if (tag->GetSymbol()->IsSectionRelative())
                    {
                        ObjExpression* left = Factory->MakeExpression(Section);
                        ObjExpression* right = Factory->MakeExpression(offset);
                        tag->GetSymbol()->SetOffset(Factory->MakeExpression(ObjExpression::eAdd, left, right));
                    }
                }
            }
        }
        offset += mem->GetSize();
    }
}
