/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 */

#include "ObjExpression.h"
#include "ObjSymbol.h"
#include "ObjSection.h"
#include "ObjMemory.h"
#include "ObjDebugTag.h"
#include "ObjFactory.h"

void ObjMemory::SetData(ObjByte *Data, ObjInt Size)
{
    if (data)
        delete[] data;
    data = new ObjByte[Size];
    if (data != nullptr)
    {
        memcpy(data, Data, Size);
        size = Size;
    }
    else
    {
        size = 0;
    }
    if (fixup)
    {
        delete fixup;
        fixup = nullptr;
    }
}
void ObjMemory::SetData(ObjExpression *Data, ObjInt Size)
{
    fixup = Data;
    size = Size;
    if (data)
    {
        delete data;
        data = nullptr;
    }
}
void ObjMemoryManager::ResolveSymbols(ObjFactory *Factory, ObjSection *Section)
{
    int offset = 0;
    for (MemoryIterator itm = MemoryBegin(); itm != MemoryEnd();++itm)
    {
        if ((*itm)->HasDebugTags())
        {
            for (ObjMemory::DebugTagIterator itd = (*itm)->DebugTagBegin();
                 itd != (*itm)->DebugTagEnd(); ++itd)
            {
                if ((*itd)->GetType() == ObjDebugTag::eVar)
                {
                    if ((*itd)->GetSymbol()->IsSectionRelative())
                    {
                        ObjExpression *left = Factory->MakeExpression(Section);
                        ObjExpression *right = Factory->MakeExpression(offset);
                        (*itd)->GetSymbol()->SetOffset(Factory->MakeExpression(ObjExpression::eAdd, left, right));
                    }
                }
            }
        }
          offset += (*itm)->GetSize();
    }
}
