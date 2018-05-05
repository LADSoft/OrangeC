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
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include "PEObject.h"
#include "Utils.h"
#include "ObjExpression.h"
#include "ObjFile.h"
#include <string.h>

void PEFixupObject::Setup(ObjInt &endVa, ObjInt &endPhys)
{
    if (virtual_addr == 0)
    {
        virtual_addr = endVa;
    }
    else
    {
        if (virtual_addr != endVa)
            Utils::fatal("Internal error");
    }
    raw_addr = endPhys;
    if (fixups.size() == 0)
    {
        // for WINNT, he needs some reloc data even if it is empty...
        Block block;
        block.rva = 0;
        block.size = 12;
        block.data[0] = block.data[1] = 0;
        data = new unsigned char[block.size];
        memcpy(data, &block, block.size);
        size = initSize = block.size;
    }
    else
    {
        int base = -1;
        initSize = 0;
        for (auto fixup : fixups)
        {
            if ((base ^ fixup) & ~4095)
            {
                if (initSize & 2)
                    initSize += 2;
                initSize += 8;
                base = fixup;
            }
            initSize += 2;
        }
        if (initSize & 2)
            initSize += 2;
        size = initSize;
        data = new unsigned char[initSize];
        memset(data, 0, initSize);
        // we relied on the set implementation to sort the fixups...
        int curSize = 0;
        Block *block = (Block *)data;
        for (auto it = fixups.begin(); it != fixups.end();)
        {
            ObjInt base = (*it) & ~(4096 - 1);
            int current = 0;
            block->rva = base - imageBase;
            block->size = 8;
            while (it != fixups.end() && ((*it) & ~(4096 - 1)) == base)
            {
                block->size+=2;
                block->data[current++] = ((*it) & (4096 -1)) + ( PE_FIXUP_HIGHLOW << 12);
                ++it;
            }
            if (block->size & 2)
            {
                block->size += 2;
                block->data[current++] = 0;
            }
            block = (Block *)((unsigned char *)block + block->size);
        }
    }
    endVa = ObjectAlign(objectAlign, endVa + size);
    endPhys = ObjectAlign(fileAlign, endPhys + initSize);
}
bool PEFixupObject::IsRel(ObjExpression *e)
{
    if (!e)
        return false;
    if (e->GetOperator() == ObjExpression::ePC)
        return true;
    return IsRel(e->GetLeft()) || IsRel(e->GetRight());
}
void PEFixupObject::LoadFixups()
{
    for (ObjFile::SectionIterator it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        ObjInt base = (*it)->GetOffset()->Eval(0);
        ObjMemoryManager &m = (*it)->GetMemoryManager();
        int ofs = 0;
        for (ObjMemoryManager::MemoryIterator it = m.MemoryBegin(); it != m.MemoryEnd(); ++it)
        {
            int msize = (*it)->GetSize();
            ObjByte *mdata = (*it)->GetData();
            if (msize)
            {
                ObjExpression *fixup = (*it)->GetFixup();
                if (fixup)
                {
                    if (msize != 4)
                        Utils::fatal("Invalid fixup type");
                    if (!IsRel(fixup))
                        fixups.insert(base + ofs);
                }
                ofs += msize;
            }
        }		
    }
}
