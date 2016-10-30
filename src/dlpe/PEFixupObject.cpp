/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
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
