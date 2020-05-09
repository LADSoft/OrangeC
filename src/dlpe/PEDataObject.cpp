/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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
 */

#include <fstream>
#include "PEObject.h"
#include "ObjExpression.h"
#include "ObjFile.h"
#include "ObjUtil.h"
#include "Utils.h"
#include "ObjSymbol.h"
#include <cstring>

void PEDataObject::Setup(ObjInt& endVa, ObjInt& endPhys)
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
    data = std::make_unique<unsigned char[]>(initSize + importCount * 6);
    memset(data.get(), 0, initSize + importCount * 6);
}
bool PEDataObject::hasPC(ObjExpression* exp)
{
    if (exp->GetOperator() == ObjExpression::ePC)
        return true;
    if (exp->GetLeft() && hasPC(exp->GetLeft()))
        return true;
    if (exp->GetRight() && hasPC(exp->GetRight()))
        return true;
    return false;
}
ObjExpression* PEDataObject::getExtern(ObjExpression* exp)
{
    if (exp->GetOperator() == ObjExpression::eSymbol)
        return exp;
    if (exp->GetLeft())
    {
        ObjExpression* s = getExtern(exp->GetLeft());
        if (s)
            return s;
    }
    if (exp->GetRight())
    {
        ObjExpression* s = getExtern(exp->GetRight());
        if (s)
            return s;
    }
    return nullptr;
}
void PEDataObject::GetImportNames()
{
    if (importNames.empty())
    {
        for (auto it = file->ImportBegin(); it != file->ImportEnd(); ++it)
        {
            importNames.insert((*it)->GetName());
        }
    }
}
ObjInt PEDataObject::EvalFixup(ObjExpression* fixup, ObjInt base)
{
    // all this is so that we can thunk relative branches so they can still
    // use the import table.
    // note I'm not reviewing how the thunk table space gets allocated, but, I
    // think it is allocated in the linker and is likely to be sparse with the
    // current implementation that minimizes its use.
    if (hasPC(fixup))
    {
        ObjExpression* ext = getExtern(fixup);
        if (ext)
        {
            ObjSymbol* sym = ext->GetSymbol();
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
    unsigned char* pdata = data.get();
    ObjMemoryManager& m = sect->GetMemoryManager();
    int ofs = 0;
    bool hasVA = name == ".text";
    int top = importThunkVA - virtual_addr - imageBase;
    for (auto it = m.MemoryBegin(); it != m.MemoryEnd(); ++it)
    {
        int msize = (*it)->GetSize();
        ObjByte* mdata = (*it)->GetData();
        if (hasVA && msize + ofs > top)
            msize = top - ofs;
        if (msize + ofs > initSize)
            msize = initSize - ofs;
        if (msize >= 0)
        {
            ObjExpression* fixup = (*it)->GetFixup();
            if (fixup)
            {
                int sbase = sect->GetOffset()->Eval(0);
                int n = EvalFixup(fixup, sbase + ofs);
                int bigEndian = file->GetBigEndian();
                if (msize == 1)
                {
                    pdata[ofs] = n & 0xff;
                }
                else if (msize == 2)
                {
                    if (bigEndian)
                    {
                        pdata[ofs] = n >> 8;
                        pdata[ofs + 1] = n & 0xff;
                    }
                    else
                    {
                        pdata[ofs] = n & 0xff;
                        pdata[ofs + 1] = n >> 8;
                    }
                }
                else  // msize == 4
                {
                    if (bigEndian)
                    {
                        pdata[ofs + 0] = n >> 24;
                        pdata[ofs + 1] = n >> 16;
                        pdata[ofs + 2] = n >> 8;
                        pdata[ofs + 3] = n & 0xff;
                    }
                    else
                    {
                        pdata[ofs] = n & 0xff;
                        pdata[ofs + 1] = n >> 8;
                        pdata[ofs + 2] = n >> 16;
                        pdata[ofs + 3] = n >> 24;
                    }
                }
            }
            else
            {
                if ((*it)->IsEnumerated())
                    memset(pdata + ofs, (*it)->GetFill(), msize);
                else
                    memcpy(pdata + ofs, mdata, msize);
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
        unsigned char* pdata = data.get();
        unsigned offs = importThunkVA - virtual_addr - imageBase + index * 6;
        pdata[offs] = 0xff;
        pdata[offs + 1] = 0x25;
        *(unsigned*)(pdata + offs + 2) = va;
        return offs + virtual_addr + imageBase;
    }
    return -1;
}
