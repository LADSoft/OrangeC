/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
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
#include <fstream>
#include "PEObject.h"
#include "ObjExpression.h"
#include "ObjFile.h"
#include "ObjUtil.h"
#include "Utils.h"
#include "ObjSymbol.h"
#include <string.h>

void PEDataObject::Setup(ObjInt &endVa, ObjInt &endPhys)
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
    endVa = ObjectAlign(objectAlign, endVa + size);
    endPhys = ObjectAlign(fileAlign, endPhys + initSize);
    data = new unsigned char[initSize];
}
bool PEDataObject::hasPC(ObjExpression *exp)
{
    if (exp->GetOperator() == ObjExpression::ePC)
        return true;
    if (exp->GetLeft() && hasPC(exp->GetLeft()))
        return true;
    if (exp->GetRight() && hasPC(exp->GetRight()))
        return true;
    return false;
}
ObjExpression* PEDataObject::getExtern(ObjExpression *exp)
{
    if (exp->GetOperator() == ObjExpression::eSymbol)
        return exp;
    if (exp->GetLeft())
    {
        ObjExpression *s = getExtern(exp->GetLeft());
        if (s)
            return s;
    }
    if (exp->GetRight())
    {
        ObjExpression *s = getExtern(exp->GetRight());
        if (s)
            return s;
    }
    return NULL;
}
void PEDataObject::GetImportNames()
{
    if (!importNames.size())
    {
        for (ObjFile::SymbolIterator it = file->ImportBegin(); it != file->ImportEnd(); ++it)
        {
            importNames.insert((*it)->GetName());
        }
    }
}
ObjInt PEDataObject::EvalFixup(ObjExpression *fixup, ObjInt base)
{
    // all this is so that we can thunk relative branches so they can still
    // use the import table.
    // note I'm not reviewing how the thunk table space gets allocated, but, I
    // think it is allocated in the linker and is likely to be sparse with the
    // current implementation that minimizes its use.
    if (hasPC(fixup))
    {
        ObjExpression * ext = getExtern(fixup);
        if (ext)
        {
            ObjSymbol *sym = ext->GetSymbol();
            if (sym->GetType() == ObjSymbol::eExternal)
            {
                GetImportNames();
                if (importNames.find(sym->GetName()) != importNames.end())
                {
                    ObjInt val1 = fixup->Eval(base);
                    ObjInt val;
                    int en = sym->GetIndex();
                    if ((val = SetThunk(en, sym->GetOffset()->Eval(0))) != -1)
                    {
                        val1 = val1 - sym->GetOffset()->Eval(0) + val;
                        return val1;
                    }
                }   
            }
        }
    }
    return fixup->Eval(base);
}
void PEDataObject::Fill()
{
    ObjMemoryManager &m = sect->GetMemoryManager();
    int ofs = 0;
    bool hasVA = name == ".text";
    int top = importThunkVA - virtual_addr - imageBase;
    for (ObjMemoryManager::MemoryIterator it = m.MemoryBegin(); it != m.MemoryEnd(); ++it)
    {
        int msize = (*it)->GetSize();
        ObjByte *mdata = (*it)->GetData();
        if (hasVA && msize + ofs > top)
            msize = top - ofs;
        if (msize + ofs > initSize)
            msize = initSize - ofs;
        if (msize >= 0)
        {
            ObjExpression *fixup = (*it)->GetFixup();
            if (fixup)
            {
                int sbase = sect->GetOffset()->Eval(0);
                int n = EvalFixup(fixup, sbase + ofs);
                int bigEndian = file->GetBigEndian();
                if (msize == 1)
                {
                    data[ofs] = n & 0xff;
                }
                else if (msize == 2)
                {
                    if (bigEndian)
                    {
                        data[ofs] = n >> 8;
                        data[ofs + 1] = n & 0xff;
                    }
                    else
                    {
                        data[ofs] = n & 0xff;
                        data[ofs+1] = n >> 8;
                    }
                }
                else // msize == 4
                {
                    if (bigEndian)
                    {
                        data[ofs + 0] = n >> 24;
                        data[ofs + 1] = n >> 16;
                        data[ofs + 2] = n >>  8;
                        data[ofs + 3] = n & 0xff;
                    }
                    else
                    {
                        data[ofs] = n & 0xff;
                        data[ofs+1] = n >> 8;
                        data[ofs+2] = n >> 16;
                        data[ofs+3] = n >> 24;
                    }
                }
            }
            else
            {
                if ((*it)->IsEnumerated())
                    memset(data + ofs, (*it)->GetFill(), msize);
                else
                    memcpy(data + ofs, mdata, msize);
            }
            ofs += msize;
        }
    }
}
void PEDataObject::InitFlags()
{
    initSize = size = sect->GetSize()->Eval(0);
    virtual_addr = sect->GetOffset()->Eval(0) - imageBase;
    if (name == ".text")
    {
        SetFlags(WINF_CODE | WINF_EXECUTE | WINF_READABLE | WINF_NEG_FLAGS);

    }
    else if (name == ".data")
    {
        SetFlags(WINF_INITDATA | WINF_READABLE | WINF_WRITEABLE | WINF_NEG_FLAGS);
        initSize = dataInitSize;
    }
    else
    {
        SetFlags(WINF_INITDATA | WINF_READABLE | WINF_WRITEABLE | WINF_NEG_FLAGS);
    }
}
ObjInt PEDataObject::SetThunk(int index, unsigned va)
{
    if (name == ".text")
    {
        unsigned offs = importThunkVA - virtual_addr - imageBase + index * 6;
        data[offs] = 0xff;
        data[offs+1] = 0x25;
        *(unsigned *)(data +offs+2) = va;
        return offs + virtual_addr + imageBase;
    }
    return -1;
}
