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

#include "PEObject.h"
#include "Utils.h"
#include "ObjExpression.h"
#include "ObjFile.h"
#include <cstring>

void PEFixupObject::Setup(ObjInt& endVa, ObjInt& endPhys)
{
    if (virtual_addr == 0)
    {
        virtual_addr = endVa;
    }
    else
    {
        if (virtual_addr != endVa)
            Utils::Fatal("Internal error");
    }
    raw_addr = endPhys;
    for (auto t : thunkFixups)
        fixups.insert(t);
    if (fixups.empty())
    {
        // for WINNT, he needs some reloc data even if it is empty...
        Block block;
        block.rva = 0;
        block.size = 12;
        block.data[0] = block.data[1] = 0;
        data = std::shared_ptr<unsigned char>(new unsigned char[block.size]);
        memcpy(data.get(), &block, block.size);
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
        data = std::shared_ptr<unsigned char>(new unsigned char[initSize]);
        std::fill(data.get(), data.get() + initSize, 0);
        // we relied on the set implementation to sort the fixups...
        int curSize = 0;
        Block* block = reinterpret_cast<Block*>(data.get());
        for (auto it = fixups.begin(); it != fixups.end();)
        {
            ObjInt base = (*it) & ~(4096 - 1);
            int current = 0;
            block->rva = base - imageBase;
            block->size = 8;
            while (it != fixups.end() && ((*it) & ~(4096 - 1)) == base)
            {
                block->size += 2;
                block->data[current++] = ((*it) & (4096 - 1)) + (PE_FIXUP_HIGHLOW << 12);
                ++it;
            }
            if (block->size & 2)
            {
                block->size += 2;
                block->data[current++] = 0;
            }
            block = (Block*)((unsigned char*)block + block->size);
        }
    }
    endVa = ObjectAlign(objectAlign, endVa + size);
    endPhys = ObjectAlign(fileAlign, endPhys + initSize);
}
bool PEFixupObject::IsRel(ObjExpression* e)
{
    if (!e)
        return false;
    if (e->GetOperator() == ObjExpression::ePC)
        return true;
    return IsRel(e->GetLeft()) || IsRel(e->GetRight());
}
bool PEFixupObject::IsInternal(ObjExpression* e)
{
    int pos = 0, neg = 0;
    CountSections(e, true, pos, neg);
    return pos == neg;
}

void PEFixupObject::CountSections(ObjExpression* e, bool positive, int& pos, int& neg)
{
    switch (e->GetOperator())
    {
        case ObjExpression::eAdd:
            CountSections(e->GetLeft(), positive, pos, neg);
            CountSections(e->GetRight(), positive, pos, neg);
            break;
        case ObjExpression::eSub:
            CountSections(e->GetLeft(), positive, pos, neg);
            CountSections(e->GetRight(), !positive, pos, neg);
            break;
        case ObjExpression::eSection:
            if (positive)
                pos++;
            else
                neg++;
            break;
    }
}
void PEFixupObject::LoadFixups()
{
    for (auto it = file->SectionBegin(); it != file->SectionEnd(); ++it)
    {
        ObjInt base = (*it)->GetOffset()->Eval(0);
        ObjMemoryManager& m = (*it)->GetMemoryManager();
        int ofs = 0;
        for (auto mem : m)
        {
            int msize = mem->GetSize();
            ObjByte* mdata = mem->GetData();
            if (msize)
            {
                ObjExpression* fixup = mem->GetFixup();
                if (fixup)
                {
                    if (msize != 4)
                        Utils::Fatal("Invalid fixup type");
                    if (!IsRel(fixup) && !IsInternal(fixup))
                        fixups.insert(base + ofs);
                }
                ofs += msize;
            }
        }
    }
}
